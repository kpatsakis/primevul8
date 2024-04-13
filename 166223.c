int http_request_forward_body(struct stream *s, struct channel *req, int an_bit)
{
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &txn->req;
	struct htx *htx;
	short status = 0;
	int ret;

	DBG_TRACE_ENTER(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn, msg);

	htx = htxbuf(&req->buf);

	if (htx->flags & HTX_FL_PARSING_ERROR)
		goto return_bad_req;
	if (htx->flags & HTX_FL_PROCESSING_ERROR)
		goto return_int_err;

	if ((req->flags & (CF_READ_ERROR|CF_READ_TIMEOUT|CF_WRITE_ERROR|CF_WRITE_TIMEOUT)) ||
	    ((req->flags & CF_SHUTW) && (req->to_forward || co_data(req)))) {
		/* Output closed while we were sending data. We must abort and
		 * wake the other side up.
		 *
		 * If we have finished to send the request and the response is
		 * still in progress, don't catch write error on the request
		 * side if it is in fact a read error on the server side.
		 */
		if (msg->msg_state == HTTP_MSG_DONE && (s->res.flags & CF_READ_ERROR) && s->res.analysers)
			return 0;

		/* Don't abort yet if we had L7 retries activated and it
		 * was a write error, we may recover.
		 */
		if (!(req->flags & (CF_READ_ERROR | CF_READ_TIMEOUT)) &&
		    (s->si[1].flags & SI_FL_L7_RETRY)) {
			DBG_TRACE_DEVEL("leaving on L7 retry",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}
		msg->msg_state = HTTP_MSG_ERROR;
		http_end_request(s);
		http_end_response(s);
		DBG_TRACE_DEVEL("leaving on error",
				STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
		return 1;
	}

	/* Note that we don't have to send 100-continue back because we don't
	 * need the data to complete our job, and it's up to the server to
	 * decide whether to return 100, 417 or anything else in return of
	 * an "Expect: 100-continue" header.
	 */
	if (msg->msg_state == HTTP_MSG_BODY)
		msg->msg_state = HTTP_MSG_DATA;

	/* in most states, we should abort in case of early close */
	channel_auto_close(req);

	if (req->to_forward) {
		if (req->to_forward == CHN_INFINITE_FORWARD) {
			if (req->flags & CF_EOI)
				msg->msg_state = HTTP_MSG_ENDING;
		}
		else {
			/* We can't process the buffer's contents yet */
			req->flags |= CF_WAKE_WRITE;
			goto missing_data_or_waiting;
		}
	}

	if (msg->msg_state >= HTTP_MSG_ENDING)
		goto ending;

	if (txn->meth == HTTP_METH_CONNECT) {
		msg->msg_state = HTTP_MSG_ENDING;
		goto ending;
	}

	/* Forward input data. We get it by removing all outgoing data not
	 * forwarded yet from HTX data size. If there are some data filters, we
	 * let them decide the amount of data to forward.
	 */
	if (HAS_REQ_DATA_FILTERS(s)) {
		ret  = flt_http_payload(s, msg, htx->data);
		if (ret < 0)
			goto return_bad_req;
		c_adv(req, ret);
	}
	else {
		c_adv(req, htx->data - co_data(req));
		if (msg->flags & HTTP_MSGF_XFER_LEN)
			channel_htx_forward_forever(req, htx);
	}

	if (htx->data != co_data(req))
		goto missing_data_or_waiting;

	/* Check if the end-of-message is reached and if so, switch the message
	 * in HTTP_MSG_ENDING state. Then if all data was marked to be
	 * forwarded, set the state to HTTP_MSG_DONE.
	 */
	if (!(htx->flags & HTX_FL_EOM))
		goto missing_data_or_waiting;

	msg->msg_state = HTTP_MSG_ENDING;

  ending:
	req->flags &= ~CF_EXPECT_MORE; /* no more data are expected */

	/* other states, ENDING...TUNNEL */
	if (msg->msg_state >= HTTP_MSG_DONE)
		goto done;

	if (HAS_REQ_DATA_FILTERS(s)) {
		ret = flt_http_end(s, msg);
		if (ret <= 0) {
			if (!ret)
				goto missing_data_or_waiting;
			goto return_bad_req;
		}
	}

	if (txn->meth == HTTP_METH_CONNECT)
		msg->msg_state = HTTP_MSG_TUNNEL;
	else {
		msg->msg_state = HTTP_MSG_DONE;
		req->to_forward = 0;
	}

  done:
	/* we don't want to forward closes on DONE except in tunnel mode. */
	if (!(txn->flags & TX_CON_WANT_TUN))
		channel_dont_close(req);

	http_end_request(s);
	if (!(req->analysers & an_bit)) {
		http_end_response(s);
		if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
			if (req->flags & CF_SHUTW) {
				/* request errors are most likely due to the
				 * server aborting the transfer. */
				goto return_srv_abort;
			}
			goto return_bad_req;
		}
		DBG_TRACE_LEAVE(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
		return 1;
	}

	/* If "option abortonclose" is set on the backend, we want to monitor
	 * the client's connection and forward any shutdown notification to the
	 * server, which will decide whether to close or to go on processing the
	 * request. We only do that in tunnel mode, and not in other modes since
	 * it can be abused to exhaust source ports. */
	if (s->be->options & PR_O_ABRT_CLOSE) {
		channel_auto_read(req);
		if ((req->flags & (CF_SHUTR|CF_READ_NULL)) && !(txn->flags & TX_CON_WANT_TUN))
			s->si[1].flags |= SI_FL_NOLINGER;
		channel_auto_close(req);
	}
	else if (s->txn->meth == HTTP_METH_POST) {
		/* POST requests may require to read extra CRLF sent by broken
		 * browsers and which could cause an RST to be sent upon close
		 * on some systems (eg: Linux). */
		channel_auto_read(req);
	}
	DBG_TRACE_DEVEL("waiting for the end of the HTTP txn",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;

 missing_data_or_waiting:
	/* stop waiting for data if the input is closed before the end */
	if (msg->msg_state < HTTP_MSG_ENDING && req->flags & CF_SHUTR)
		goto return_cli_abort;

 waiting:
	/* waiting for the last bits to leave the buffer */
	if (req->flags & CF_SHUTW)
		goto return_srv_abort;

	/* When TE: chunked is used, we need to get there again to parse remaining
	 * chunks even if the client has closed, so we don't want to set CF_DONTCLOSE.
	 * And when content-length is used, we never want to let the possible
	 * shutdown be forwarded to the other side, as the state machine will
	 * take care of it once the client responds. It's also important to
	 * prevent TIME_WAITs from accumulating on the backend side, and for
	 * HTTP/2 where the last frame comes with a shutdown.
	 */
	if (msg->flags & HTTP_MSGF_XFER_LEN)
		channel_dont_close(req);

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the CF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if (HAS_REQ_DATA_FILTERS(s))
		req->flags |= CF_EXPECT_MORE;

	DBG_TRACE_DEVEL("waiting for more data to forward",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;

  return_cli_abort:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.cli_aborts);
	_HA_ATOMIC_INC(&s->be->be_counters.cli_aborts);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->cli_aborts);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.cli_aborts);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_CLICL;
	status = 400;
	goto return_prx_cond;

  return_srv_abort:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.srv_aborts);
	_HA_ATOMIC_INC(&s->be->be_counters.srv_aborts);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->srv_aborts);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.srv_aborts);
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_SRVCL;
	status = 502;
	goto return_prx_cond;

  return_int_err:
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_INTERNAL;
	_HA_ATOMIC_INC(&sess->fe->fe_counters.internal_errors);
	_HA_ATOMIC_INC(&s->be->be_counters.internal_errors);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->internal_errors);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.internal_errors);
	status = 500;
	goto return_prx_cond;

  return_bad_req:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.failed_req);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->failed_req);
	status = 400;
	/* fall through */

  return_prx_cond:
	if (txn->status > 0) {
		/* Note: we don't send any error if some data were already sent */
		http_reply_and_close(s, txn->status, NULL);
	} else {
		txn->status = status;
		http_reply_and_close(s, txn->status, http_error_message(s));
	}
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_PRXCOND;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= ((txn->rsp.msg_state < HTTP_MSG_ERROR) ? SF_FINST_H : SF_FINST_D);
	DBG_TRACE_DEVEL("leaving on error ",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
	return 0;
}