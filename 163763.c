int http_response_forward_body(struct session *s, struct channel *res, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &s->txn.rsp;
	unsigned int bytes;
	static struct buffer *tmpbuf = NULL;
	int compressing = 0;
	int consumed_data = 0;
	int ret;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))
		return 0;

	if ((res->flags & (CF_READ_ERROR|CF_READ_TIMEOUT|CF_WRITE_ERROR|CF_WRITE_TIMEOUT)) ||
	    ((res->flags & CF_SHUTW) && (res->to_forward || res->buf->o)) ||
	    !s->req->analysers) {
		/* Output closed while we were sending data. We must abort and
		 * wake the other side up.
		 */
		msg->msg_state = HTTP_MSG_ERROR;
		http_resync_states(s);
		return 1;
	}

	/* in most states, we should abort in case of early close */
	channel_auto_close(res);

	/* this is the first time we need the compression buffer */
	if (s->comp_algo != NULL && tmpbuf == NULL) {
		if ((tmpbuf = pool_alloc2(pool2_buffer)) == NULL)
			goto aborted_xfer; /* no memory */
	}

	if (msg->msg_state < HTTP_MSG_CHUNK_SIZE) {
		/* we have msg->sov which points to the first byte of message body.
		 * rep->buf.p still points to the beginning of the message and msg->sol
		 * is still null. We forward the headers, we don't need them.
		 */
		channel_forward(res, msg->sov);
		msg->next = 0;
		msg->sov = 0;

		if (msg->flags & HTTP_MSGF_TE_CHNK)
			msg->msg_state = HTTP_MSG_CHUNK_SIZE;
		else
			msg->msg_state = HTTP_MSG_DATA;
	}

	if (s->comp_algo != NULL) {
		ret = http_compression_buffer_init(s, res->buf, tmpbuf); /* init a buffer with headers */
		if (ret < 0)
			goto missing_data; /* not enough spaces in buffers */
		compressing = 1;
	}

	while (1) {
		http_silent_debug(__LINE__, s);
		/* we may have some data pending between sol and sov */
		if (s->comp_algo == NULL) {
			bytes = msg->sov - msg->sol;
			if (msg->chunk_len || bytes) {
				msg->sol = msg->sov;
				msg->next -= bytes; /* will be forwarded */
				msg->chunk_len += bytes;
				msg->chunk_len -= channel_forward(res, msg->chunk_len);
			}
		}

		switch (msg->msg_state - HTTP_MSG_DATA) {
		case HTTP_MSG_DATA - HTTP_MSG_DATA:	/* must still forward */
			if (compressing) {
				consumed_data += ret = http_compression_buffer_add_data(s, res->buf, tmpbuf);
				if (ret < 0)
					goto aborted_xfer;
			}

			if (res->to_forward || msg->chunk_len)
				goto missing_data;

			/* nothing left to forward */
			if (msg->flags & HTTP_MSGF_TE_CHNK) {
				msg->msg_state = HTTP_MSG_CHUNK_CRLF;
			} else {
				msg->msg_state = HTTP_MSG_DONE;
				if (compressing && consumed_data) {
					http_compression_buffer_end(s, &res->buf, &tmpbuf, 1);
					compressing = 0;
				}
				break;
			}
			/* fall through for HTTP_MSG_CHUNK_CRLF */

		case HTTP_MSG_CHUNK_CRLF - HTTP_MSG_DATA:
			/* we want the CRLF after the data */

			ret = http_skip_chunk_crlf(msg);
			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, msg, HTTP_MSG_CHUNK_CRLF, s->fe);
				goto return_bad_res;
			}
			/* skipping data in buffer for compression */
			if (compressing) {
				b_adv(res->buf, msg->next);
				msg->next = 0;
				msg->sov = 0;
				msg->sol = 0;
			}
			/* we're in MSG_CHUNK_SIZE now, fall through */

		case HTTP_MSG_CHUNK_SIZE - HTTP_MSG_DATA:
			/* read the chunk size and assign it to ->chunk_len, then
			 * set ->sov and ->next to point to the body and switch to DATA or
			 * TRAILERS state.
			 */

			ret = http_parse_chunk_size(msg);
			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, msg, HTTP_MSG_CHUNK_SIZE, s->fe);
				goto return_bad_res;
			}
			if (compressing) {
				if (likely(msg->chunk_len > 0)) {
					/* skipping data if we are in compression mode */
					b_adv(res->buf, msg->next);
					msg->next = 0;
					msg->sov = 0;
					msg->sol = 0;
				} else {
					if (consumed_data) {
						http_compression_buffer_end(s, &res->buf, &tmpbuf, 1);
						compressing = 0;
					}
				}
			}
			/* otherwise we're in HTTP_MSG_DATA or HTTP_MSG_TRAILERS state */
			break;

		case HTTP_MSG_TRAILERS - HTTP_MSG_DATA:
			ret = http_forward_trailers(msg);
			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, msg, HTTP_MSG_TRAILERS, s->fe);
				goto return_bad_res;
			}
			if (s->comp_algo != NULL) {
				/* forwarding trailers */
				channel_forward(res, msg->next);
				msg->next = 0;
			}
			/* we're in HTTP_MSG_DONE now, but we might still have
			 * some data pending, so let's loop over once.
			 */
			break;

		default:
			/* other states, DONE...TUNNEL */

			ret = msg->msg_state;
			/* for keep-alive we don't want to forward closes on DONE */
			if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
			    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL)
				channel_dont_close(res);
			if (http_resync_states(s)) {
				http_silent_debug(__LINE__, s);
				/* some state changes occurred, maybe the analyser
				 * was disabled too.
				 */
				if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
					if (res->flags & CF_SHUTW) {
						/* response errors are most likely due to
						 * the client aborting the transfer.
						 */
						goto aborted_xfer;
					}
					if (msg->err_pos >= 0)
						http_capture_bad_message(&s->be->invalid_rep, s, msg, ret, s->fe);
					goto return_bad_res;
				}
				return 1;
			}
			return 0;
		}
	}

 missing_data:
	if (compressing && consumed_data) {
		http_compression_buffer_end(s, &res->buf, &tmpbuf, 0);
		compressing = 0;
	}

	if (res->flags & CF_SHUTW)
		goto aborted_xfer;

	/* stop waiting for data if the input is closed before the end. If the
	 * client side was already closed, it means that the client has aborted,
	 * so we don't want to count this as a server abort. Otherwise it's a
	 * server abort.
	 */
	if (res->flags & CF_SHUTR) {
		if ((res->flags & CF_SHUTW_NOW) || (s->req->flags & CF_SHUTR))
			goto aborted_xfer;
		if (!(s->flags & SN_ERR_MASK))
			s->flags |= SN_ERR_SRVCL;
		s->be->be_counters.srv_aborts++;
		if (objt_server(s->target))
			objt_server(s->target)->counters.srv_aborts++;
		goto return_bad_res_stats_ok;
	}

	/* we need to obey the req analyser, so if it leaves, we must too */
	if (!s->req->analysers)
		goto return_bad_res;

	/* forward any data pending between sol and sov */
	if (s->comp_algo == NULL) {
		bytes = msg->sov - msg->sol;
		if (msg->chunk_len || bytes) {
			msg->sol = msg->sov;
			msg->next -= bytes; /* will be forwarded */
			msg->chunk_len += bytes;
			msg->chunk_len -= channel_forward(res, msg->chunk_len);
		}
	}

	/* When TE: chunked is used, we need to get there again to parse remaining
	 * chunks even if the server has closed, so we don't want to set CF_DONTCLOSE.
	 * Similarly, with keep-alive on the client side, we don't want to forward a
	 * close.
	 */
	if ((msg->flags & HTTP_MSGF_TE_CHNK) || s->comp_algo ||
	    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
	    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL)
		channel_dont_close(res);

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the CF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if ((msg->flags & HTTP_MSGF_TE_CHNK) || s->comp_algo)
		res->flags |= CF_EXPECT_MORE;

	/* the session handler will take care of timeouts and errors */
	http_silent_debug(__LINE__, s);
	return 0;

 return_bad_res: /* let's centralize all bad responses */
	s->be->be_counters.failed_resp++;
	if (objt_server(s->target))
		objt_server(s->target)->counters.failed_resp++;

 return_bad_res_stats_ok:
	txn->rsp.msg_state = HTTP_MSG_ERROR;
	/* don't send any error message as we're in the body */
	stream_int_retnclose(res->cons, NULL);
	res->analysers = 0;
	s->req->analysers = 0; /* we're in data phase, we want to abort both directions */
	if (objt_server(s->target))
		health_adjust(objt_server(s->target), HANA_STATUS_HTTP_HDRRSP);

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_D;
	return 0;

 aborted_xfer:
	txn->rsp.msg_state = HTTP_MSG_ERROR;
	/* don't send any error message as we're in the body */
	stream_int_retnclose(res->cons, NULL);
	res->analysers = 0;
	s->req->analysers = 0; /* we're in data phase, we want to abort both directions */

	s->fe->fe_counters.cli_aborts++;
	s->be->be_counters.cli_aborts++;
	if (objt_server(s->target))
		objt_server(s->target)->counters.cli_aborts++;

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_CLICL;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_D;
	return 0;
}