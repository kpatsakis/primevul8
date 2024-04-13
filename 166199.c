int http_wait_for_response(struct stream *s, struct channel *rep, int an_bit)
{
	/*
	 * We will analyze a complete HTTP response to check the its syntax.
	 *
	 * Once the start line and all headers are received, we may perform a
	 * capture of the error (if any), and we will set a few fields. We also
	 * logging and finally headers capture.
	 */
	struct session *sess = s->sess;
	struct http_txn *txn = s->txn;
	struct http_msg *msg = &txn->rsp;
	struct htx *htx;
	struct stream_interface *si_b = &s->si[1];
	struct connection *srv_conn;
	struct htx_sl *sl;
	int n;

	DBG_TRACE_ENTER(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn, msg);

	htx = htxbuf(&rep->buf);

	/* Parsing errors are caught here */
	if (htx->flags & HTX_FL_PARSING_ERROR)
		goto return_bad_res;
	if (htx->flags & HTX_FL_PROCESSING_ERROR)
		goto return_int_err;

	/*
	 * Now we quickly check if we have found a full valid response.
	 * If not so, we check the FD and buffer states before leaving.
	 * A full response is indicated by the fact that we have seen
	 * the double LF/CRLF, so the state is >= HTTP_MSG_BODY. Invalid
	 * responses are checked first.
	 *
	 * Depending on whether the client is still there or not, we
	 * may send an error response back or not. Note that normally
	 * we should only check for HTTP status there, and check I/O
	 * errors somewhere else.
	 */
  next_one:
	if (unlikely(htx_is_empty(htx) || htx->first == -1)) {
		/* 1: have we encountered a read error ? */
		if (rep->flags & CF_READ_ERROR) {
			struct connection *conn = NULL;

			if (objt_cs(s->si[1].end))
				conn = __objt_cs(s->si[1].end)->conn;

			/* Perform a L7 retry because server refuses the early data. */
			if ((si_b->flags & SI_FL_L7_RETRY) &&
			    (s->be->retry_type & PR_RE_EARLY_ERROR) &&
			    conn && conn->err_code == CO_ER_SSL_EARLY_FAILED &&
			    do_l7_retry(s, si_b) == 0) {
				DBG_TRACE_DEVEL("leaving on L7 retry",
						STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
				return 0;
			}

			if (txn->flags & TX_NOT_FIRST)
				goto abort_keep_alive;

			_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
			if (objt_server(s->target)) {
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
				health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_READ_ERROR);
			}

			/* if the server refused the early data, just send a 425 */
			if (conn && conn->err_code == CO_ER_SSL_EARLY_FAILED)
				txn->status = 425;
			else {
				txn->status = 502;
				stream_inc_http_fail_ctr(s);
			}

			s->si[1].flags |= SI_FL_NOLINGER;
			http_reply_and_close(s, txn->status, http_error_message(s));

			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_SRVCL;
			if (!(s->flags & SF_FINST_MASK))
				s->flags |= SF_FINST_H;
			DBG_TRACE_DEVEL("leaving on error",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}

		/* 2: read timeout : return a 504 to the client. */
		else if (rep->flags & CF_READ_TIMEOUT) {
			if ((si_b->flags & SI_FL_L7_RETRY) &&
			    (s->be->retry_type & PR_RE_TIMEOUT)) {
				if (co_data(rep) || do_l7_retry(s, si_b) == 0) {
					DBG_TRACE_DEVEL("leaving on L7 retry",
							STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
					return 0;
				}
			}
			_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
			if (objt_server(s->target)) {
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
				health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_READ_TIMEOUT);
			}

			txn->status = 504;
			stream_inc_http_fail_ctr(s);
			s->si[1].flags |= SI_FL_NOLINGER;
			http_reply_and_close(s, txn->status, http_error_message(s));

			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_SRVTO;
			if (!(s->flags & SF_FINST_MASK))
				s->flags |= SF_FINST_H;
			DBG_TRACE_DEVEL("leaving on error",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}

		/* 3: client abort with an abortonclose */
		else if ((rep->flags & CF_SHUTR) && ((s->req.flags & (CF_SHUTR|CF_SHUTW)) == (CF_SHUTR|CF_SHUTW))) {
			_HA_ATOMIC_INC(&sess->fe->fe_counters.cli_aborts);
			_HA_ATOMIC_INC(&s->be->be_counters.cli_aborts);
			if (sess->listener && sess->listener->counters)
				_HA_ATOMIC_INC(&sess->listener->counters->cli_aborts);
			if (objt_server(s->target))
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.cli_aborts);

			txn->status = 400;
			http_reply_and_close(s, txn->status, http_error_message(s));

			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_CLICL;
			if (!(s->flags & SF_FINST_MASK))
				s->flags |= SF_FINST_H;

			/* process_stream() will take care of the error */
			DBG_TRACE_DEVEL("leaving on error",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}

		/* 4: close from server, capture the response if the server has started to respond */
		else if (rep->flags & CF_SHUTR) {
			if ((si_b->flags & SI_FL_L7_RETRY) &&
			    (s->be->retry_type & PR_RE_DISCONNECTED)) {
				if (co_data(rep) || do_l7_retry(s, si_b) == 0) {
					DBG_TRACE_DEVEL("leaving on L7 retry",
							STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
					return 0;
				}
			}

			if (txn->flags & TX_NOT_FIRST)
				goto abort_keep_alive;

			_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
			if (objt_server(s->target)) {
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
				health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_BROKEN_PIPE);
			}

			txn->status = 502;
			stream_inc_http_fail_ctr(s);
			s->si[1].flags |= SI_FL_NOLINGER;
			http_reply_and_close(s, txn->status, http_error_message(s));

			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_SRVCL;
			if (!(s->flags & SF_FINST_MASK))
				s->flags |= SF_FINST_H;
			DBG_TRACE_DEVEL("leaving on error",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}

		/* 5: write error to client (we don't send any message then) */
		else if (rep->flags & CF_WRITE_ERROR) {
			if (txn->flags & TX_NOT_FIRST)
				goto abort_keep_alive;

			_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
			if (objt_server(s->target))
				_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
			rep->analysers &= AN_RES_FLT_END;

			if (!(s->flags & SF_ERR_MASK))
				s->flags |= SF_ERR_CLICL;
			if (!(s->flags & SF_FINST_MASK))
				s->flags |= SF_FINST_H;

			/* process_stream() will take care of the error */
			DBG_TRACE_DEVEL("leaving on error",
					STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
			return 0;
		}

		channel_dont_close(rep);
		rep->flags |= CF_READ_DONTWAIT; /* try to get back here ASAP */
		DBG_TRACE_DEVEL("waiting for more data",
				STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
		return 0;
	}

	/* More interesting part now : we know that we have a complete
	 * response which at least looks like HTTP. We have an indicator
	 * of each header's length, so we can parse them quickly.
	 */
	BUG_ON(htx_get_first_type(htx) != HTX_BLK_RES_SL);
	sl = http_get_stline(htx);

	/* Perform a L7 retry because of the status code */
	if ((si_b->flags & SI_FL_L7_RETRY) &&
	    l7_status_match(s->be, sl->info.res.status) &&
	    do_l7_retry(s, si_b) == 0) {
		DBG_TRACE_DEVEL("leaving on L7 retry", STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
		return 0;
	}

	/* Now, L7 buffer is useless, it can be released */
	b_free(&s->si[1].l7_buffer);

	msg->msg_state = HTTP_MSG_BODY;


	/* 0: we might have to print this header in debug mode */
	if (unlikely((global.mode & MODE_DEBUG) &&
		     (!(global.mode & MODE_QUIET) || (global.mode & MODE_VERBOSE)))) {
		int32_t pos;

		http_debug_stline("srvrep", s, sl);

		for (pos = htx_get_first(htx); pos != -1; pos = htx_get_next(htx, pos)) {
			struct htx_blk *blk = htx_get_blk(htx, pos);
			enum htx_blk_type type = htx_get_blk_type(blk);

			if (type == HTX_BLK_EOH)
				break;
			if (type != HTX_BLK_HDR)
				continue;

			http_debug_hdr("srvhdr", s,
				       htx_get_blk_name(htx, blk),
				       htx_get_blk_value(htx, blk));
		}
	}

	/* 1: get the status code and the version. Also set HTTP flags */
	txn->status = sl->info.res.status;
	if (sl->flags & HTX_SL_F_VER_11)
                msg->flags |= HTTP_MSGF_VER_11;
	if (sl->flags & HTX_SL_F_XFER_LEN) {
		msg->flags |= HTTP_MSGF_XFER_LEN;
		if (sl->flags & HTX_SL_F_CLEN)
			msg->flags |= HTTP_MSGF_CNT_LEN;
		else if (sl->flags & HTX_SL_F_CHNK)
			msg->flags |= HTTP_MSGF_TE_CHNK;
	}
	if (sl->flags & HTX_SL_F_BODYLESS)
		msg->flags |= HTTP_MSGF_BODYLESS;
	if (sl->flags & HTX_SL_F_CONN_UPG)
		msg->flags |= HTTP_MSGF_CONN_UPG;

	n = txn->status / 100;
	if (n < 1 || n > 5)
		n = 0;

	/* when the client triggers a 4xx from the server, it's most often due
	 * to a missing object or permission. These events should be tracked
	 * because if they happen often, it may indicate a brute force or a
	 * vulnerability scan.
	 */
	if (n == 4)
		stream_inc_http_err_ctr(s);

	if (n == 5 && txn->status != 501 && txn->status != 505)
		stream_inc_http_fail_ctr(s);

	if (objt_server(s->target)) {
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.p.http.rsp[n]);
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.p.http.cum_req);
	}

	/* Adjust server's health based on status code. Note: status codes 501
	 * and 505 are triggered on demand by client request, so we must not
	 * count them as server failures.
	 */
	if (objt_server(s->target)) {
		if (txn->status >= 100 && (txn->status < 500 || txn->status == 501 || txn->status == 505))
			health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_OK);
		else
			health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_STS);
	}

	/*
	 * We may be facing a 100-continue response, or any other informational
	 * 1xx response which is non-final, in which case this is not the right
	 * response, and we're waiting for the next one. Let's allow this response
	 * to go to the client and wait for the next one. There's an exception for
	 * 101 which is used later in the code to switch protocols.
	 */
	if (txn->status < 200 &&
	    (txn->status == 100 || txn->status >= 102)) {
		FLT_STRM_CB(s, flt_http_reset(s, msg));
		htx->first = channel_htx_fwd_headers(rep, htx);
		msg->msg_state = HTTP_MSG_RPBEFORE;
		msg->flags = 0;
		txn->status = 0;
		s->logs.t_data = -1; /* was not a response yet */
		rep->flags |= CF_SEND_DONTWAIT; /* Send ASAP informational messages */
		goto next_one;
	}

	/* A 101-switching-protocols must contains a Connection header with the
	 * "upgrade" option and the request too. It means both are agree to
	 * upgrade. It is not so strict because there is no test on the Upgrade
	 * header content. But it is probably stronger enough for now.
	 */
	if (txn->status == 101 &&
	    (!(txn->req.flags & HTTP_MSGF_CONN_UPG) || !(txn->rsp.flags & HTTP_MSGF_CONN_UPG)))
		goto return_bad_res;

	/*
	 * 2: check for cacheability.
	 */

	switch (txn->status) {
	case 200:
	case 203:
	case 204:
	case 206:
	case 300:
	case 301:
	case 404:
	case 405:
	case 410:
	case 414:
	case 501:
		break;
	default:
		/* RFC7231#6.1:
		 *   Responses with status codes that are defined as
		 *   cacheable by default (e.g., 200, 203, 204, 206,
		 *   300, 301, 404, 405, 410, 414, and 501 in this
		 *   specification) can be reused by a cache with
		 *   heuristic expiration unless otherwise indicated
		 *   by the method definition or explicit cache
		 *   controls [RFC7234]; all other status codes are
		 *   not cacheable by default.
		 */
		txn->flags &= ~(TX_CACHEABLE | TX_CACHE_COOK);
		break;
	}

	/*
	 * 3: we may need to capture headers
	 */
	s->logs.logwait &= ~LW_RESP;
	if (unlikely((s->logs.logwait & LW_RSPHDR) && s->res_cap))
		http_capture_headers(htx, s->res_cap, sess->fe->rsp_cap);

	/* Skip parsing if no content length is possible. */
	if (unlikely((txn->meth == HTTP_METH_CONNECT && txn->status >= 200 && txn->status < 300) ||
		     txn->status == 101)) {
		/* Either we've established an explicit tunnel, or we're
		 * switching the protocol. In both cases, we're very unlikely
		 * to understand the next protocols. We have to switch to tunnel
		 * mode, so that we transfer the request and responses then let
		 * this protocol pass unmodified. When we later implement specific
		 * parsers for such protocols, we'll want to check the Upgrade
		 * header which contains information about that protocol for
		 * responses with status 101 (eg: see RFC2817 about TLS).
		 */
		txn->flags |= TX_CON_WANT_TUN;
	}

	/* check for NTML authentication headers in 401 (WWW-Authenticate) and
	 * 407 (Proxy-Authenticate) responses and set the connection to private
	 */
	srv_conn = cs_conn(objt_cs(s->si[1].end));
	if (srv_conn) {
		struct ist hdr;
		struct http_hdr_ctx ctx;

		if (txn->status == 401)
			hdr = ist("WWW-Authenticate");
		else if (txn->status == 407)
			hdr = ist("Proxy-Authenticate");
		else
			goto end;

		ctx.blk = NULL;
		while (http_find_header(htx, hdr, &ctx, 0)) {
			/* If www-authenticate contains "Negotiate", "Nego2", or "NTLM",
			 * possibly followed by blanks and a base64 string, the connection
			 * is private. Since it's a mess to deal with, we only check for
			 * values starting with "NTLM" or "Nego". Note that often multiple
			 * headers are sent by the server there.
			 */
			if ((ctx.value.len >= 4 && strncasecmp(ctx.value.ptr, "Nego", 4) == 0) ||
			    (ctx.value.len >= 4 && strncasecmp(ctx.value.ptr, "NTLM", 4) == 0)) {
				sess->flags |= SESS_FL_PREFER_LAST;
				conn_set_owner(srv_conn, sess, NULL);
				conn_set_private(srv_conn);
				/* If it fail now, the same will be done in mux->detach() callback */
				session_add_conn(srv_conn->owner, srv_conn, srv_conn->target);
				break;
			}
		}
	}

  end:
	/* we want to have the response time before we start processing it */
	s->logs.t_data = tv_ms_elapsed(&s->logs.tv_accept, &now);

	/* end of job, return OK */
	rep->analysers &= ~an_bit;
	rep->analyse_exp = TICK_ETERNITY;
	channel_auto_close(rep);
	DBG_TRACE_LEAVE(STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 1;

 return_int_err:
	_HA_ATOMIC_INC(&sess->fe->fe_counters.internal_errors);
	_HA_ATOMIC_INC(&s->be->be_counters.internal_errors);
	if (sess->listener && sess->listener->counters)
		_HA_ATOMIC_INC(&sess->listener->counters->internal_errors);
	if (objt_server(s->target))
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.internal_errors);
	txn->status = 500;
	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_INTERNAL;
	goto return_prx_cond;

  return_bad_res:
	_HA_ATOMIC_INC(&s->be->be_counters.failed_resp);
	if (objt_server(s->target)) {
		_HA_ATOMIC_INC(&__objt_server(s->target)->counters.failed_resp);
		health_adjust(__objt_server(s->target), HANA_STATUS_HTTP_HDRRSP);
	}
	if ((s->be->retry_type & PR_RE_JUNK_REQUEST) &&
	    (si_b->flags & SI_FL_L7_RETRY) &&
	    do_l7_retry(s, si_b) == 0) {
		DBG_TRACE_DEVEL("leaving on L7 retry",
				STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
		return 0;
	}
	txn->status = 502;
	stream_inc_http_fail_ctr(s);
	/* fall through */

 return_prx_cond:
	http_reply_and_close(s, txn->status, http_error_message(s));

	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_PRXCOND;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= SF_FINST_H;

	s->si[1].flags |= SI_FL_NOLINGER;
	DBG_TRACE_DEVEL("leaving on error",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA|STRM_EV_HTTP_ERR, s, txn);
	return 0;

 abort_keep_alive:
	/* A keep-alive request to the server failed on a network error.
	 * The client is required to retry. We need to close without returning
	 * any other information so that the client retries.
	 */
	txn->status = 0;
	s->logs.logwait = 0;
	s->logs.level = 0;
	s->res.flags &= ~CF_EXPECT_MORE; /* speed up sending a previous response */
	http_reply_and_close(s, txn->status, NULL);
	DBG_TRACE_DEVEL("leaving by closing K/A connection",
			STRM_EV_STRM_ANA|STRM_EV_HTTP_ANA, s, txn);
	return 0;
}