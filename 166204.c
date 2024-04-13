int http_response_forward_body(struct stream *s, struct channel *res, int an_bit)
{
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &s->txn->rsp;
	struct htx *htx;
	int ret;

	DBG_TRACE_ENTER(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn, msg);

	htx = htxbuf(&res->buf);

	if (htx->flags & HTX_FL_PARSING_ERROR)
		goto return_bad_res;
	if (htx->flags & HTX_FL_PROCESSING_ERROR)
		goto return_int_err;

	if ((res->flags & (CF_READ_ERROR|CF_READ_TIMEOUT|CF_WRITE_ERROR|CF_WRITE_TIMEOUT)) ||
	    ((res->flags & CF_SHUTW) && (res->to_forward || co_data(res)))) {
		/* Output closed while we were sending data. We must abort and
		 * wake the other side up.
		 */
		msg->msg_state = HTTP_MSG_ERROR;
		http_end_response(s);
		http_end_request(s);
		DBG_TRACE_DEVEL("leaving on error",
				STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
		return 1;
	}

	if (msg->msg_state == HTTP_MSG_BODY)
		msg->msg_state = HTTP_MSG_DATA;

	/* in most states, we should abort in case of early close */
	channel_auto_close(res);

	if (res->to_forward) {
		if (res->to_forward == CHN_INFINITE_FORWARD) {
			if (res->flags & CF_EOI)
				msg->msg_state = HTTP_MSG_ENDING;
		}
		else {
			/* We can't process the buffer's contents yet */
			res->flags |= CF_WAKE_WRITE;
			goto missing_data_or_waiting;
		}
	}

	if (msg->msg_state >= HTTP_MSG_ENDING)
		goto ending;

	if ((txn->meth == HTTP_METH_CONNECT && txn->status >= 200 && txn->status < 300) || txn->status == 101 ||
	    (!(msg->flags & HTTP_MSGF_XFER_LEN) && !HAS_RSP_DATA_FILTERS(s))) {
		msg->msg_state = HTTP_MSG_ENDING;
		goto ending;
	}

	/* Forward input data. We get it by removing all outgoing data not
	 * forwarded yet from HTX data size. If there are some data filters, we
	 * let them decide the amount of data to forward.
	 */
	if (HAS_RSP_DATA_FILTERS(s)) {
		ret  = flt_http_payload(s, msg, htx->data);
		if (ret < 0)
			goto return_bad_res;
		c_adv(res, ret);
	}
	else {
		c_adv(res, htx->data - co_data(res));
		if (msg->flags & HTTP_MSGF_XFER_LEN)
			channel_htx_forward_forever(res, htx);
	}

	if (htx->data != co_data(res))
		goto missing_data_or_waiting;

	if (!(msg->flags & HTTP_MSGF_XFER_LEN) && res->flags & CF_SHUTR) {
		msg->msg_state = HTTP_MSG_ENDING;
		goto ending;
	}

	/* Check if the end-of-message is reached and if so, switch the message
	 * in HTTP_MSG_ENDING state. Then if all data was marked to be
	 * forwarded, set the state to HTTP_MSG_DONE.
	 */
	if (!(htx->flags & HTX_FL_EOM))
		goto missing_data_or_waiting;

	msg->msg_state = HTTP_MSG_ENDING;

  ending:
	res->flags &= ~CF_EXPECT_MORE; /* no more data are expected */

	/* other states, ENDING...TUNNEL */
	if (msg->msg_state >= HTTP_MSG_DONE)
		goto done;

	if (HAS_RSP_DATA_FILTERS(s)) {
		ret = flt_http_end(s, msg);
		if (ret <= 0) {
			if (!ret)
				goto missing_data_or_waiting;
			goto return_bad_res;
		}
	}

	if ((txn->meth == HTTP_METH_CONNECT && txn->status >= 200 && txn->status < 300) || txn->status == 101 ||
	    !(msg->flags & HTTP_MSGF_XFER_LEN)) {
		msg->msg_state = HTTP_MSG_TUNNEL;
		goto ending;
	}
	else {
		msg->msg_state = HTTP_MSG_DONE;
		res->to_forward = 0;
	}

  done:

	channel_dont_close(res);

	http_end_response(s);
	if (!(res->analysers & an_bit)) {
		http_end_request(s);
		if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
			if (res->flags & CF_SHUTW) {
				/* response errors are most likely due to the
				 * client aborting the transfer. */
				goto return_cli_abort;
			}
			goto return_bad_res;
		}
		DBG_TRACE_LEAVE(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
		return 1;
	}
	DBG_TRACE_DEVEL("waiting for the end of the HTTP txn",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;

  missing_data_or_waiting:
	if (res->flags & CF_SHUTW)
		goto return_cli_abort;

	/* stop waiting for data if the input is closed before the end. If the
	 * client side was already closed, it means that the client has aborted,
	 * so we don't want to count this as a server abort. Otherwise it's a
	 * server abort.
	 */
	if (msg->msg_state < HTTP_MSG_ENDING && res->flags & CF_SHUTR) {
		if ((s->req.flags & (CF_SHUTR|CF_SHUTW)) == (CF_SHUTR|CF_SHUTW))
			goto return_cli_abort;
		/* If we have some pending data, we continue the processing */
		if (htx_is_empty(htx))
			goto return_srv_abort;
	}

	/* When TE: chunked is used, we need to get there again to parse
	 * remaining chunks even if the server has closed, so we don't want to
	 * set CF_DONTCLOSE. Similarly when there is a content-leng or if there
	 * are filters registered on the stream, we don't want to forward a
	 * close
	 */
	if ((msg->flags & HTTP_MSGF_XFER_LEN) || HAS_RSP_DATA_FILTERS(s))
		channel_dont_close(res);

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the CF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if (HAS_RSP_DATA_FILTERS(s))
		res->flags |= CF_EXPECT_MORE;

	/* the stream handler will take care of timeouts and errors */
	DBG_TRACE_DEVEL("waiting for more data to forward",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;

  return_srv_abort:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.srv_aborts);
	_HA_ATOMIC_INC(&s->be->be_counters.srv_aborts);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->srv_aborts);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.srv_aborts);
	stream_inc_http_fail_ctr(s);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_SRVCL;
	goto return_error;

  return_cli_abort:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.cli_aborts);
	_HA_ATOMIC_INC(&s->be->be_counters.cli_aborts);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->cli_aborts);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.cli_aborts);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_CLICL;
	goto return_error;

  return_int_err:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.internal_errors);
	_HA_ATOMIC_INC(&s->be->be_counters.internal_errors);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->internal_errors);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.internal_errors);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_INTERNAL;
	goto return_error;

  return_bad_res:
	_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
	if (objt_server(s->target)) {
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
		health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_RSP);
	}
	stream_inc_http_fail_ctr(s);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_SRVCL;
	/* fall through */

   return_error:
	/* don't send any error message as we're in the body */
	http_reply_and_close(s, txn->status, NULL);
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= SF_FINST_D;
	DBG_TRACE_DEVEL("leaving on error",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
	return 0;
}