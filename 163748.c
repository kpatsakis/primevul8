int http_wait_for_request(struct session *s, struct channel *req, int an_bit)
{
	/*
	 * We will parse the partial (or complete) lines.
	 * We will check the request syntax, and also join multi-line
	 * headers. An index of all the lines will be elaborated while
	 * parsing.
	 *
	 * For the parsing, we use a 28 states FSM.
	 *
	 * Here is the information we currently have :
	 *   req->buf->p             = beginning of request
	 *   req->buf->p + msg->eoh  = end of processed headers / start of current one
	 *   req->buf->p + req->buf->i    = end of input data
	 *   msg->eol           = end of current header or line (LF or CRLF)
	 *   msg->next          = first non-visited byte
	 *
	 * At end of parsing, we may perform a capture of the error (if any), and
	 * we will set a few fields (msg->sol, txn->meth, sn->flags/SN_REDIRECTABLE).
	 * We also check for monitor-uri, logging, HTTP/0.9 to 1.0 conversion, and
	 * finally headers capture.
	 */

	int cur_idx;
	int use_close_only;
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;
	struct hdr_ctx ctx;

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bh=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		s,
		req,
		req->rex, req->wex,
		req->flags,
		req->buf->i,
		req->analysers);

	/* we're speaking HTTP here, so let's speak HTTP to the client */
	s->srv_error = http_return_srv_error;

	/* There's a protected area at the end of the buffer for rewriting
	 * purposes. We don't want to start to parse the request if the
	 * protected area is affected, because we may have to move processed
	 * data later, which is much more complicated.
	 */
	if (buffer_not_empty(req->buf) && msg->msg_state < HTTP_MSG_ERROR) {
		if ((txn->flags & TX_NOT_FIRST) &&
		    unlikely(channel_full(req) ||
			     bi_end(req->buf) < b_ptr(req->buf, msg->next) ||
			     bi_end(req->buf) > req->buf->data + req->buf->size - global.tune.maxrewrite)) {
			if (req->buf->o) {
				if (req->flags & (CF_SHUTW|CF_SHUTW_NOW|CF_WRITE_ERROR|CF_WRITE_TIMEOUT))
					goto failed_keep_alive;
				/* some data has still not left the buffer, wake us once that's done */
				channel_dont_connect(req);
				req->flags |= CF_READ_DONTWAIT; /* try to get back here ASAP */
				return 0;
			}
			if (bi_end(req->buf) < b_ptr(req->buf, msg->next) ||
			    bi_end(req->buf) > req->buf->data + req->buf->size - global.tune.maxrewrite)
				buffer_slow_realign(msg->chn->buf);
		}

		/* Note that we have the same problem with the response ; we
		 * may want to send a redirect, error or anything which requires
		 * some spare space. So we'll ensure that we have at least
		 * maxrewrite bytes available in the response buffer before
		 * processing that one. This will only affect pipelined
		 * keep-alive requests.
		 */
		if ((txn->flags & TX_NOT_FIRST) &&
		    unlikely(channel_full(s->rep) ||
			     bi_end(s->rep->buf) < b_ptr(s->rep->buf, txn->rsp.next) ||
			     bi_end(s->rep->buf) > s->rep->buf->data + s->rep->buf->size - global.tune.maxrewrite)) {
			if (s->rep->buf->o) {
				if (s->rep->flags & (CF_SHUTW|CF_SHUTW_NOW|CF_WRITE_ERROR|CF_WRITE_TIMEOUT))
					goto failed_keep_alive;
				/* don't let a connection request be initiated */
				channel_dont_connect(req);
				s->rep->flags &= ~CF_EXPECT_MORE; /* speed up sending a previous response */
				s->rep->analysers |= an_bit; /* wake us up once it changes */
				return 0;
			}
		}

		if (likely(msg->next < req->buf->i)) /* some unparsed data are available */
			http_msg_analyzer(msg, &txn->hdr_idx);
	}

	/* 1: we might have to print this header in debug mode */
	if (unlikely((global.mode & MODE_DEBUG) &&
		     (!(global.mode & MODE_QUIET) || (global.mode & MODE_VERBOSE)) &&
		     (msg->msg_state >= HTTP_MSG_BODY || msg->msg_state == HTTP_MSG_ERROR))) {
		char *eol, *sol;

		sol = req->buf->p;
		/* this is a bit complex : in case of error on the request line,
		 * we know that rq.l is still zero, so we display only the part
		 * up to the end of the line (truncated by debug_hdr).
		 */
		eol = sol + (msg->sl.rq.l ? msg->sl.rq.l : req->buf->i);
		debug_hdr("clireq", s, sol, eol);

		sol += hdr_idx_first_pos(&txn->hdr_idx);
		cur_idx = hdr_idx_first_idx(&txn->hdr_idx);

		while (cur_idx) {
			eol = sol + txn->hdr_idx.v[cur_idx].len;
			debug_hdr("clihdr", s, sol, eol);
			sol = eol + txn->hdr_idx.v[cur_idx].cr + 1;
			cur_idx = txn->hdr_idx.v[cur_idx].next;
		}
	}


	/*
	 * Now we quickly check if we have found a full valid request.
	 * If not so, we check the FD and buffer states before leaving.
	 * A full request is indicated by the fact that we have seen
	 * the double LF/CRLF, so the state is >= HTTP_MSG_BODY. Invalid
	 * requests are checked first. When waiting for a second request
	 * on a keep-alive session, if we encounter and error, close, t/o,
	 * we note the error in the session flags but don't set any state.
	 * Since the error will be noted there, it will not be counted by
	 * process_session() as a frontend error.
	 * Last, we may increase some tracked counters' http request errors on
	 * the cases that are deliberately the client's fault. For instance,
	 * a timeout or connection reset is not counted as an error. However
	 * a bad request is.
	 */

	if (unlikely(msg->msg_state < HTTP_MSG_BODY)) {
		/*
		 * First, let's catch bad requests.
		 */
		if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
			session_inc_http_req_ctr(s);
			session_inc_http_err_ctr(s);
			proxy_inc_fe_req_ctr(s->fe);
			goto return_bad_req;
		}

		/* 1: Since we are in header mode, if there's no space
		 *    left for headers, we won't be able to free more
		 *    later, so the session will never terminate. We
		 *    must terminate it now.
		 */
		if (unlikely(buffer_full(req->buf, global.tune.maxrewrite))) {
			/* FIXME: check if URI is set and return Status
			 * 414 Request URI too long instead.
			 */
			session_inc_http_req_ctr(s);
			session_inc_http_err_ctr(s);
			proxy_inc_fe_req_ctr(s->fe);
			if (msg->err_pos < 0)
				msg->err_pos = req->buf->i;
			goto return_bad_req;
		}

		/* 2: have we encountered a read error ? */
		else if (req->flags & CF_READ_ERROR) {
			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_CLICL;

			if (txn->flags & TX_WAIT_NEXT_RQ)
				goto failed_keep_alive;

			/* we cannot return any message on error */
			if (msg->err_pos >= 0) {
				http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
				session_inc_http_err_ctr(s);
			}

			txn->status = 400;
			stream_int_retnclose(req->prod, NULL);
			msg->msg_state = HTTP_MSG_ERROR;
			req->analysers = 0;

			session_inc_http_req_ctr(s);
			proxy_inc_fe_req_ctr(s->fe);
			s->fe->fe_counters.failed_req++;
			if (s->listener->counters)
				s->listener->counters->failed_req++;

			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_R;
			return 0;
		}

		/* 3: has the read timeout expired ? */
		else if (req->flags & CF_READ_TIMEOUT || tick_is_expired(req->analyse_exp, now_ms)) {
			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_CLITO;

			if (txn->flags & TX_WAIT_NEXT_RQ)
				goto failed_keep_alive;

			/* read timeout : give up with an error message. */
			if (msg->err_pos >= 0) {
				http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
				session_inc_http_err_ctr(s);
			}
			txn->status = 408;
			stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_408));
			msg->msg_state = HTTP_MSG_ERROR;
			req->analysers = 0;

			session_inc_http_req_ctr(s);
			proxy_inc_fe_req_ctr(s->fe);
			s->fe->fe_counters.failed_req++;
			if (s->listener->counters)
				s->listener->counters->failed_req++;

			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_R;
			return 0;
		}

		/* 4: have we encountered a close ? */
		else if (req->flags & CF_SHUTR) {
			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_CLICL;

			if (txn->flags & TX_WAIT_NEXT_RQ)
				goto failed_keep_alive;

			if (msg->err_pos >= 0)
				http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
			txn->status = 400;
			stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));
			msg->msg_state = HTTP_MSG_ERROR;
			req->analysers = 0;

			session_inc_http_err_ctr(s);
			session_inc_http_req_ctr(s);
			proxy_inc_fe_req_ctr(s->fe);
			s->fe->fe_counters.failed_req++;
			if (s->listener->counters)
				s->listener->counters->failed_req++;

			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_R;
			return 0;
		}

		channel_dont_connect(req);
		req->flags |= CF_READ_DONTWAIT; /* try to get back here ASAP */
		s->rep->flags &= ~CF_EXPECT_MORE; /* speed up sending a previous response */
#ifdef TCP_QUICKACK
		if (s->listener->options & LI_O_NOQUICKACK && req->buf->i) {
			/* We need more data, we have to re-enable quick-ack in case we
			 * previously disabled it, otherwise we might cause the client
			 * to delay next data.
			 */
			setsockopt(s->si[0].conn->t.sock.fd, IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));
		}
#endif

		if ((msg->msg_state != HTTP_MSG_RQBEFORE) && (txn->flags & TX_WAIT_NEXT_RQ)) {
			/* If the client starts to talk, let's fall back to
			 * request timeout processing.
			 */
			txn->flags &= ~TX_WAIT_NEXT_RQ;
			req->analyse_exp = TICK_ETERNITY;
		}

		/* just set the request timeout once at the beginning of the request */
		if (!tick_isset(req->analyse_exp)) {
			if ((msg->msg_state == HTTP_MSG_RQBEFORE) &&
			    (txn->flags & TX_WAIT_NEXT_RQ) &&
			    tick_isset(s->be->timeout.httpka))
				req->analyse_exp = tick_add(now_ms, s->be->timeout.httpka);
			else
				req->analyse_exp = tick_add_ifset(now_ms, s->be->timeout.httpreq);
		}

		/* we're not ready yet */
		return 0;

	failed_keep_alive:
		/* Here we process low-level errors for keep-alive requests. In
		 * short, if the request is not the first one and it experiences
		 * a timeout, read error or shutdown, we just silently close so
		 * that the client can try again.
		 */
		txn->status = 0;
		msg->msg_state = HTTP_MSG_RQBEFORE;
		req->analysers = 0;
		s->logs.logwait = 0;
		s->rep->flags &= ~CF_EXPECT_MORE; /* speed up sending a previous response */
		stream_int_retnclose(req->prod, NULL);
		return 0;
	}

	/* OK now we have a complete HTTP request with indexed headers. Let's
	 * complete the request parsing by setting a few fields we will need
	 * later. At this point, we have the last CRLF at req->buf->data + msg->eoh.
	 * If the request is in HTTP/0.9 form, the rule is still true, and eoh
	 * points to the CRLF of the request line. msg->next points to the first
	 * byte after the last LF. msg->sov points to the first byte of data.
	 * msg->eol cannot be trusted because it may have been left uninitialized
	 * (for instance in the absence of headers).
	 */

	session_inc_http_req_ctr(s);
	proxy_inc_fe_req_ctr(s->fe); /* one more valid request for this FE */

	if (txn->flags & TX_WAIT_NEXT_RQ) {
		/* kill the pending keep-alive timeout */
		txn->flags &= ~TX_WAIT_NEXT_RQ;
		req->analyse_exp = TICK_ETERNITY;
	}


	/* Maybe we found in invalid header name while we were configured not
	 * to block on that, so we have to capture it now.
	 */
	if (unlikely(msg->err_pos >= 0))
		http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);

	/*
	 * 1: identify the method
	 */
	txn->meth = find_http_meth(req->buf->p, msg->sl.rq.m_l);

	/* we can make use of server redirect on GET and HEAD */
	if (txn->meth == HTTP_METH_GET || txn->meth == HTTP_METH_HEAD)
		s->flags |= SN_REDIRECTABLE;

	/*
	 * 2: check if the URI matches the monitor_uri.
	 * We have to do this for every request which gets in, because
	 * the monitor-uri is defined by the frontend.
	 */
	if (unlikely((s->fe->monitor_uri_len != 0) &&
		     (s->fe->monitor_uri_len == msg->sl.rq.u_l) &&
		     !memcmp(req->buf->p + msg->sl.rq.u,
			     s->fe->monitor_uri,
			     s->fe->monitor_uri_len))) {
		/*
		 * We have found the monitor URI
		 */
		struct acl_cond *cond;

		s->flags |= SN_MONITOR;
		s->fe->fe_counters.intercepted_req++;

		/* Check if we want to fail this monitor request or not */
		list_for_each_entry(cond, &s->fe->mon_fail_cond, list) {
			int ret = acl_exec_cond(cond, s->fe, s, txn, SMP_OPT_DIR_REQ|SMP_OPT_FINAL);

			ret = acl_pass(ret);
			if (cond->pol == ACL_COND_UNLESS)
				ret = !ret;

			if (ret) {
				/* we fail this request, let's return 503 service unavail */
				txn->status = 503;
				stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_503));
				goto return_prx_cond;
			}
		}

		/* nothing to fail, let's reply normaly */
		txn->status = 200;
		stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_200));
		goto return_prx_cond;
	}

	/*
	 * 3: Maybe we have to copy the original REQURI for the logs ?
	 * Note: we cannot log anymore if the request has been
	 * classified as invalid.
	 */
	if (unlikely(s->logs.logwait & LW_REQ)) {
		/* we have a complete HTTP request that we must log */
		if ((txn->uri = pool_alloc2(pool2_requri)) != NULL) {
			int urilen = msg->sl.rq.l;

			if (urilen >= REQURI_LEN)
				urilen = REQURI_LEN - 1;
			memcpy(txn->uri, req->buf->p, urilen);
			txn->uri[urilen] = 0;

			if (!(s->logs.logwait &= ~(LW_REQ|LW_INIT)))
				s->do_log(s);
		} else {
			Alert("HTTP logging : out of memory.\n");
		}
	}

	if (!LIST_ISEMPTY(&s->fe->format_unique_id))
		s->unique_id = pool_alloc2(pool2_uniqueid);

	/* 4. We may have to convert HTTP/0.9 requests to HTTP/1.0 */
	if (unlikely(msg->sl.rq.v_l == 0) && !http_upgrade_v09_to_v10(txn))
		goto return_bad_req;

	/* ... and check if the request is HTTP/1.1 or above */
	if ((msg->sl.rq.v_l == 8) &&
	    ((req->buf->p[msg->sl.rq.v + 5] > '1') ||
	     ((req->buf->p[msg->sl.rq.v + 5] == '1') &&
	      (req->buf->p[msg->sl.rq.v + 7] >= '1'))))
		msg->flags |= HTTP_MSGF_VER_11;

	/* "connection" has not been parsed yet */
	txn->flags &= ~(TX_HDR_CONN_PRS | TX_HDR_CONN_CLO | TX_HDR_CONN_KAL | TX_HDR_CONN_UPG);

	/* if the frontend has "option http-use-proxy-header", we'll check if
	 * we have what looks like a proxied connection instead of a connection,
	 * and in this case set the TX_USE_PX_CONN flag to use Proxy-connection.
	 * Note that this is *not* RFC-compliant, however browsers and proxies
	 * happen to do that despite being non-standard :-(
	 * We consider that a request not beginning with either '/' or '*' is
	 * a proxied connection, which covers both "scheme://location" and
	 * CONNECT ip:port.
	 */
	if ((s->fe->options2 & PR_O2_USE_PXHDR) &&
	    req->buf->p[msg->sl.rq.u] != '/' && req->buf->p[msg->sl.rq.u] != '*')
		txn->flags |= TX_USE_PX_CONN;

	/* transfer length unknown*/
	msg->flags &= ~HTTP_MSGF_XFER_LEN;

	/* 5: we may need to capture headers */
	if (unlikely((s->logs.logwait & LW_REQHDR) && txn->req.cap))
		capture_headers(req->buf->p, &txn->hdr_idx,
				txn->req.cap, s->fe->req_cap);

	/* 6: determine the transfer-length.
	 * According to RFC2616 #4.4, amended by the HTTPbis working group,
	 * the presence of a message-body in a REQUEST and its transfer length
	 * must be determined that way (in order of precedence) :
	 *   1. The presence of a message-body in a request is signaled by the
	 *      inclusion of a Content-Length or Transfer-Encoding header field
	 *      in the request's header fields.  When a request message contains
	 *      both a message-body of non-zero length and a method that does
	 *      not define any semantics for that request message-body, then an
	 *      origin server SHOULD either ignore the message-body or respond
	 *      with an appropriate error message (e.g., 413).  A proxy or
	 *      gateway, when presented the same request, SHOULD either forward
	 *      the request inbound with the message- body or ignore the
	 *      message-body when determining a response.
	 *
	 *   2. If a Transfer-Encoding header field (Section 9.7) is present
	 *      and the "chunked" transfer-coding (Section 6.2) is used, the
	 *      transfer-length is defined by the use of this transfer-coding.
	 *      If a Transfer-Encoding header field is present and the "chunked"
	 *      transfer-coding is not present, the transfer-length is defined
	 *      by the sender closing the connection.
	 *
	 *   3. If a Content-Length header field is present, its decimal value in
	 *      OCTETs represents both the entity-length and the transfer-length.
	 *      If a message is received with both a Transfer-Encoding header
	 *      field and a Content-Length header field, the latter MUST be ignored.
	 *
	 *   4. By the server closing the connection. (Closing the connection
	 *      cannot be used to indicate the end of a request body, since that
	 *      would leave no possibility for the server to send back a response.)
	 *
	 *   Whenever a transfer-coding is applied to a message-body, the set of
	 *   transfer-codings MUST include "chunked", unless the message indicates
	 *   it is terminated by closing the connection.  When the "chunked"
	 *   transfer-coding is used, it MUST be the last transfer-coding applied
	 *   to the message-body.
	 */

	use_close_only = 0;
	ctx.idx = 0;
	/* set TE_CHNK and XFER_LEN only if "chunked" is seen last */
	while ((msg->flags & HTTP_MSGF_VER_11) &&
	       http_find_header2("Transfer-Encoding", 17, req->buf->p, &txn->hdr_idx, &ctx)) {
		if (ctx.vlen == 7 && strncasecmp(ctx.line + ctx.val, "chunked", 7) == 0)
			msg->flags |= (HTTP_MSGF_TE_CHNK | HTTP_MSGF_XFER_LEN);
		else if (msg->flags & HTTP_MSGF_TE_CHNK) {
			/* bad transfer-encoding (chunked followed by something else) */
			use_close_only = 1;
			msg->flags &= ~(HTTP_MSGF_TE_CHNK | HTTP_MSGF_XFER_LEN);
			break;
		}
	}

	ctx.idx = 0;
	while (!(msg->flags & HTTP_MSGF_TE_CHNK) && !use_close_only &&
	       http_find_header2("Content-Length", 14, req->buf->p, &txn->hdr_idx, &ctx)) {
		signed long long cl;

		if (!ctx.vlen) {
			msg->err_pos = ctx.line + ctx.val - req->buf->p;
			goto return_bad_req;
		}

		if (strl2llrc(ctx.line + ctx.val, ctx.vlen, &cl)) {
			msg->err_pos = ctx.line + ctx.val - req->buf->p;
			goto return_bad_req; /* parse failure */
		}

		if (cl < 0) {
			msg->err_pos = ctx.line + ctx.val - req->buf->p;
			goto return_bad_req;
		}

		if ((msg->flags & HTTP_MSGF_CNT_LEN) && (msg->chunk_len != cl)) {
			msg->err_pos = ctx.line + ctx.val - req->buf->p;
			goto return_bad_req; /* already specified, was different */
		}

		msg->flags |= HTTP_MSGF_CNT_LEN | HTTP_MSGF_XFER_LEN;
		msg->body_len = msg->chunk_len = cl;
	}

	/* bodyless requests have a known length */
	if (!use_close_only)
		msg->flags |= HTTP_MSGF_XFER_LEN;

	/* end of job, return OK */
	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;
	return 1;

 return_bad_req:
	/* We centralize bad requests processing here */
	if (unlikely(msg->msg_state == HTTP_MSG_ERROR) || msg->err_pos >= 0) {
		/* we detected a parsing error. We want to archive this request
		 * in the dedicated proxy area for later troubleshooting.
		 */
		http_capture_bad_message(&s->fe->invalid_req, s, msg, msg->msg_state, s->fe);
	}

	txn->req.msg_state = HTTP_MSG_ERROR;
	txn->status = 400;
	stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));

	s->fe->fe_counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;

 return_prx_cond:
	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

	req->analysers = 0;
	req->analyse_exp = TICK_ETERNITY;
	return 0;
}