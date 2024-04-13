static void http_end_response(struct stream *s)
{
	struct channel *chn = &s->res;
	struct http_txn *txn = s->txn;

	DBG_TRACE_ENTER(STRM_EV_HTTP_ANA, s, txn);

	if (unlikely(txn->req.msg_state == HTTP_MSG_ERROR ||
		     txn->rsp.msg_state == HTTP_MSG_ERROR)) {
		channel_htx_truncate(&s->req, htxbuf(&s->req.buf));
		channel_abort(&s->req);
		goto end;
	}

	if (unlikely(txn->rsp.msg_state < HTTP_MSG_DONE)) {
		DBG_TRACE_DEVEL("waiting end of the response", STRM_EV_HTTP_ANA, s, txn);
		return;
	}

	if (txn->rsp.msg_state == HTTP_MSG_DONE) {
		/* In theory, we don't need to read anymore, but we must
		 * still monitor the server connection for a possible close
		 * while the request is being uploaded, so we don't disable
		 * reading.
		 */
		/* channel_dont_read(chn); */

		if (txn->req.msg_state < HTTP_MSG_DONE) {
			/* The client seems to still be sending data, probably
			 * because we got an error response during an upload.
			 * We have the choice of either breaking the connection
			 * or letting it pass through. Let's do the later.
			 */
			DBG_TRACE_DEVEL("waiting end of the request", STRM_EV_HTTP_ANA, s, txn);
			return;
		}

		/* When we get here, it means that both the request and the
		 * response have finished receiving. Depending on the connection
		 * mode, we'll have to wait for the last bytes to leave in either
		 * direction, and sometimes for a close to be effective.
		 */
		if (txn->flags & TX_CON_WANT_TUN) {
			channel_auto_read(chn);
			if (b_data(&chn->buf)) {
				DBG_TRACE_DEVEL("waiting to flush the respone", STRM_EV_HTTP_ANA, s, txn);
				return;
			}
			txn->rsp.msg_state = HTTP_MSG_TUNNEL;
		}
		else {
			/* we're not expecting any new data to come for this
			 * transaction, so we can close it.
			 */
			if (!(chn->flags & (CF_SHUTW|CF_SHUTW_NOW))) {
				channel_shutr_now(chn);
				channel_shutw_now(chn);
			}
		}
		goto check_channel_flags;
	}

	if (txn->rsp.msg_state == HTTP_MSG_CLOSING) {
	  http_msg_closing:
		/* nothing else to forward, just waiting for the output buffer
		 * to be empty and for the shutw_now to take effect.
		 */
		if (channel_is_empty(chn)) {
			txn->rsp.msg_state = HTTP_MSG_CLOSED;
			goto http_msg_closed;
		}
		else if (chn->flags & CF_SHUTW) {
			txn->rsp.msg_state = HTTP_MSG_ERROR;
			_HA_ATOMIC_INC(&strm_sess(s)->fe->fe_counters.cli_aborts);
			_HA_ATOMIC_INC(&s->be->be_counters.cli_aborts);
			if (strm_sess(s)->listener && strm_sess(s)->listener->counters)
				_HA_ATOMIC_INC(&strm_sess(s)->listener->counters->cli_aborts);
			if (objt_server(s->target))
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.cli_aborts);
			goto end;
		}
		DBG_TRACE_LEAVE(STRM_EV_HTTP_ANA, s, txn);
		return;
	}

	if (txn->rsp.msg_state == HTTP_MSG_CLOSED) {
	  http_msg_closed:
		/* drop any pending data */
		channel_htx_truncate(&s->req, htxbuf(&s->req.buf));
		channel_abort(&s->req);
		goto end;
	}

  check_channel_flags:
	/* Here, we are in HTTP_MSG_DONE or HTTP_MSG_TUNNEL */
	if (chn->flags & (CF_SHUTW|CF_SHUTW_NOW)) {
		/* if we've just closed an output, let's switch */
		txn->rsp.msg_state = HTTP_MSG_CLOSING;
		goto http_msg_closing;
	}

  end:
	chn->analysers &= AN_RES_FLT_END;
	if (txn->rsp.msg_state == HTTP_MSG_TUNNEL) {
		chn->flags |= CF_NEVER_WAIT;
		if (HAS_RSP_DATA_FILTERS(s))
			chn->analysers |= AN_RES_FLT_XFER_DATA;
	}
	channel_auto_close(chn);
	channel_auto_read(chn);
	DBG_TRACE_LEAVE(STRM_EV_HTTP_ANA, s, txn);
}