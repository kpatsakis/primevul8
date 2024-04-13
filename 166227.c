static void http_end_request(struct stream *s)
{
	struct channel *chn = &s->req;
	struct http_txn *txn = s->txn;

	DBG_TRACE_ENTER(STRM_EV_HTTP_ANA, s, txn);

	if (unlikely(txn->req.msg_state == HTTP_MSG_ERROR ||
		     txn->rsp.msg_state == HTTP_MSG_ERROR)) {
		channel_abort(chn);
		channel_htx_truncate(chn, htxbuf(&chn->buf));
		goto end;
	}

	if (unlikely(txn->req.msg_state < HTTP_MSG_DONE)) {
		DBG_TRACE_DEVEL("waiting end of the request", STRM_EV_HTTP_ANA, s, txn);
		return;
	}

	if (txn->req.msg_state == HTTP_MSG_DONE) {
		/* No need to read anymore, the request was completely parsed.
		 * We can shut the read side unless we want to abort_on_close,
		 * or we have a POST request. The issue with POST requests is
		 * that some browsers still send a CRLF after the request, and
		 * this CRLF must be read so that it does not remain in the kernel
		 * buffers, otherwise a close could cause an RST on some systems
		 * (eg: Linux).
		 */
		if (!(s->be->options & PR_O_ABRT_CLOSE) && txn->meth != HTTP_METH_POST)
			channel_dont_read(chn);

		/* if the server closes the connection, we want to immediately react
		 * and close the socket to save packets and syscalls.
		 */
		s->si[1].flags |= SI_FL_NOHALF;

		/* In any case we've finished parsing the request so we must
		 * disable Nagle when sending data because 1) we're not going
		 * to shut this side, and 2) the server is waiting for us to
		 * send pending data.
		 */
		chn->flags |= CF_NEVER_WAIT;

		if (txn->rsp.msg_state < HTTP_MSG_DONE) {
			/* The server has not finished to respond, so we
			 * don't want to move in order not to upset it.
			 */
			DBG_TRACE_DEVEL("waiting end of the response", STRM_EV_HTTP_ANA, s, txn);
			return;
		}

		/* When we get here, it means that both the request and the
		 * response have finished receiving. Depending on the connection
		 * mode, we'll have to wait for the last bytes to leave in either
		 * direction, and sometimes for a close to be effective.
		 */
		if (txn->flags & TX_CON_WANT_TUN) {
			/* Tunnel mode will not have any analyser so it needs to
			 * poll for reads.
			 */
			channel_auto_read(chn);
			if (b_data(&chn->buf)) {
				DBG_TRACE_DEVEL("waiting to flush the request", STRM_EV_HTTP_ANA, s, txn);
				return;
			}
			txn->req.msg_state = HTTP_MSG_TUNNEL;
		}
		else {
			/* we're not expecting any new data to come for this
			 * transaction, so we can close it.
			 *
			 *  However, there is an exception if the response
			 *  length is undefined. In this case, we need to wait
			 *  the close from the server. The response will be
			 *  switched in TUNNEL mode until the end.
			 */
			if (!(txn->rsp.flags & HTTP_MSGF_XFER_LEN) &&
			    txn->rsp.msg_state != HTTP_MSG_CLOSED)
				goto check_channel_flags;

			if (!(chn->flags & (CF_SHUTW|CF_SHUTW_NOW))) {
				channel_shutr_now(chn);
				channel_shutw_now(chn);
			}
		}
		goto check_channel_flags;
	}

	if (txn->req.msg_state == HTTP_MSG_CLOSING) {
	  http_msg_closing:
		/* nothing else to forward, just waiting for the output buffer
		 * to be empty and for the shutw_now to take effect.
		 */
		if (channel_is_empty(chn)) {
			txn->req.msg_state = HTTP_MSG_CLOSED;
			goto http_msg_closed;
		}
		else if (chn->flags & CF_SHUTW) {
			txn->req.msg_state = HTTP_MSG_ERROR;
			goto end;
		}
		DBG_TRACE_LEAVE(STRM_EV_HTTP_ANA, s, txn);
		return;
	}

	if (txn->req.msg_state == HTTP_MSG_CLOSED) {
	  http_msg_closed:
		/* if we don't know whether the server will close, we need to hard close */
		if (txn->rsp.flags & HTTP_MSGF_XFER_LEN)
			s->si[1].flags |= SI_FL_NOLINGER;  /* we want to close ASAP */
		/* see above in MSG_DONE why we only do this in these states */
		if (!(s->be->options & PR_O_ABRT_CLOSE))
			channel_dont_read(chn);
		goto end;
	}

  check_channel_flags:
	/* Here, we are in HTTP_MSG_DONE or HTTP_MSG_TUNNEL */
	if (chn->flags & (CF_SHUTW|CF_SHUTW_NOW)) {
		/* if we've just closed an output, let's switch */
		txn->req.msg_state = HTTP_MSG_CLOSING;
		goto http_msg_closing;
	}

  end:
	chn->analysers &= AN_REQ_FLT_END;
	if (txn->req.msg_state == HTTP_MSG_TUNNEL) {
		chn->flags |= CF_NEVER_WAIT;
		if (HAS_REQ_DATA_FILTERS(s))
			chn->analysers |= AN_REQ_FLT_XFER_DATA;
	}
	channel_auto_close(chn);
	channel_auto_read(chn);
	DBG_TRACE_LEAVE(STRM_EV_HTTP_ANA, s, txn);
}