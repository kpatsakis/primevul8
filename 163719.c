int http_wait_for_response(struct session *s, struct channel *rep, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->rsp;
	struct hdr_ctx ctx;
	int use_close_only;
	int cur_idx;
	int n;

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bh=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		s,
		rep,
		rep->rex, rep->wex,
		rep->flags,
		rep->buf->i,
		rep->analysers);

	/*
	 * Now parse the partial (or complete) lines.
	 * We will check the response syntax, and also join multi-line
	 * headers. An index of all the lines will be elaborated while
	 * parsing.
	 *
	 * For the parsing, we use a 28 states FSM.
	 *
	 * Here is the information we currently have :
	 *   rep->buf->p             = beginning of response
	 *   rep->buf->p + msg->eoh  = end of processed headers / start of current one
	 *   rep->buf->p + rep->buf->i    = end of input data
	 *   msg->eol           = end of current header or line (LF or CRLF)
	 *   msg->next          = first non-visited byte
	 */

	/* There's a protected area at the end of the buffer for rewriting
	 * purposes. We don't want to start to parse the request if the
	 * protected area is affected, because we may have to move processed
	 * data later, which is much more complicated.
	 */
	if (buffer_not_empty(rep->buf) && msg->msg_state < HTTP_MSG_ERROR) {
		if (unlikely(channel_full(rep) ||
			     bi_end(rep->buf) < b_ptr(rep->buf, msg->next) ||
			     bi_end(rep->buf) > rep->buf->data + rep->buf->size - global.tune.maxrewrite)) {
			if (rep->buf->o) {
				/* some data has still not left the buffer, wake us once that's done */
				if (rep->flags & (CF_SHUTW|CF_SHUTW_NOW|CF_WRITE_ERROR|CF_WRITE_TIMEOUT))
					goto abort_response;
				channel_dont_close(rep);
				rep->flags |= CF_READ_DONTWAIT; /* try to get back here ASAP */
				return 0;
			}
			if (rep->buf->i <= rep->buf->size - global.tune.maxrewrite)
				buffer_slow_realign(msg->chn->buf);
		}

		if (likely(msg->next < rep->buf->i))
			http_msg_analyzer(msg, &txn->hdr_idx);
	}

	/* 1: we might have to print this header in debug mode */
	if (unlikely((global.mode & MODE_DEBUG) &&
		     (!(global.mode & MODE_QUIET) || (global.mode & MODE_VERBOSE)) &&
		     (msg->msg_state >= HTTP_MSG_BODY || msg->msg_state == HTTP_MSG_ERROR))) {
		char *eol, *sol;

		sol = rep->buf->p;
		eol = sol + (msg->sl.st.l ? msg->sl.st.l : rep->buf->i);
		debug_hdr("srvrep", s, sol, eol);

		sol += hdr_idx_first_pos(&txn->hdr_idx);
		cur_idx = hdr_idx_first_idx(&txn->hdr_idx);

		while (cur_idx) {
			eol = sol + txn->hdr_idx.v[cur_idx].len;
			debug_hdr("srvhdr", s, sol, eol);
			sol = eol + txn->hdr_idx.v[cur_idx].cr + 1;
			cur_idx = txn->hdr_idx.v[cur_idx].next;
		}
	}

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

	if (unlikely(msg->msg_state < HTTP_MSG_BODY)) {
		/* Invalid response */
		if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
			/* we detected a parsing error. We want to archive this response
			 * in the dedicated proxy area for later troubleshooting.
			 */
		hdr_response_bad:
			if (msg->msg_state == HTTP_MSG_ERROR || msg->err_pos >= 0)
				http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

			s->be->be_counters.failed_resp++;
			if (objt_server(s->target)) {
				objt_server(s->target)->counters.failed_resp++;
				health_adjust(objt_server(s->target), HANA_STATUS_HTTP_HDRRSP);
			}
		abort_response:
			channel_auto_close(rep);
			rep->analysers = 0;
			txn->status = 502;
			rep->prod->flags |= SI_FL_NOLINGER;
			bi_erase(rep);
			stream_int_retnclose(rep->cons, http_error_message(s, HTTP_ERR_502));

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_PRXCOND;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;

			return 0;
		}

		/* too large response does not fit in buffer. */
		else if (buffer_full(rep->buf, global.tune.maxrewrite)) {
			if (msg->err_pos < 0)
				msg->err_pos = rep->buf->i;
			goto hdr_response_bad;
		}

		/* read error */
		else if (rep->flags & CF_READ_ERROR) {
			if (msg->err_pos >= 0)
				http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

			s->be->be_counters.failed_resp++;
			if (objt_server(s->target)) {
				objt_server(s->target)->counters.failed_resp++;
				health_adjust(objt_server(s->target), HANA_STATUS_HTTP_READ_ERROR);
			}

			channel_auto_close(rep);
			rep->analysers = 0;
			txn->status = 502;
			rep->prod->flags |= SI_FL_NOLINGER;
			bi_erase(rep);
			stream_int_retnclose(rep->cons, http_error_message(s, HTTP_ERR_502));

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_SRVCL;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;
			return 0;
		}

		/* read timeout : return a 504 to the client. */
		else if (rep->flags & CF_READ_TIMEOUT) {
			if (msg->err_pos >= 0)
				http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

			s->be->be_counters.failed_resp++;
			if (objt_server(s->target)) {
				objt_server(s->target)->counters.failed_resp++;
				health_adjust(objt_server(s->target), HANA_STATUS_HTTP_READ_TIMEOUT);
			}

			channel_auto_close(rep);
			rep->analysers = 0;
			txn->status = 504;
			rep->prod->flags |= SI_FL_NOLINGER;
			bi_erase(rep);
			stream_int_retnclose(rep->cons, http_error_message(s, HTTP_ERR_504));

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_SRVTO;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;
			return 0;
		}

		/* client abort with an abortonclose */
		else if ((rep->flags & CF_SHUTR) && ((s->req->flags & (CF_SHUTR|CF_SHUTW)) == (CF_SHUTR|CF_SHUTW))) {
			s->fe->fe_counters.cli_aborts++;
			s->be->be_counters.cli_aborts++;
			if (objt_server(s->target))
				objt_server(s->target)->counters.cli_aborts++;

			rep->analysers = 0;
			channel_auto_close(rep);

			txn->status = 400;
			bi_erase(rep);
			stream_int_retnclose(rep->cons, http_error_message(s, HTTP_ERR_400));

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_CLICL;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;

			/* process_session() will take care of the error */
			return 0;
		}

		/* close from server, capture the response if the server has started to respond */
		else if (rep->flags & CF_SHUTR) {
			if (msg->msg_state >= HTTP_MSG_RPVER || msg->err_pos >= 0)
				http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

			s->be->be_counters.failed_resp++;
			if (objt_server(s->target)) {
				objt_server(s->target)->counters.failed_resp++;
				health_adjust(objt_server(s->target), HANA_STATUS_HTTP_BROKEN_PIPE);
			}

			channel_auto_close(rep);
			rep->analysers = 0;
			txn->status = 502;
			rep->prod->flags |= SI_FL_NOLINGER;
			bi_erase(rep);
			stream_int_retnclose(rep->cons, http_error_message(s, HTTP_ERR_502));

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_SRVCL;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;
			return 0;
		}

		/* write error to client (we don't send any message then) */
		else if (rep->flags & CF_WRITE_ERROR) {
			if (msg->err_pos >= 0)
				http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

			s->be->be_counters.failed_resp++;
			rep->analysers = 0;
			channel_auto_close(rep);

			if (!(s->flags & SN_ERR_MASK))
				s->flags |= SN_ERR_CLICL;
			if (!(s->flags & SN_FINST_MASK))
				s->flags |= SN_FINST_H;

			/* process_session() will take care of the error */
			return 0;
		}

		channel_dont_close(rep);
		return 0;
	}

	/* More interesting part now : we know that we have a complete
	 * response which at least looks like HTTP. We have an indicator
	 * of each header's length, so we can parse them quickly.
	 */

	if (unlikely(msg->err_pos >= 0))
		http_capture_bad_message(&s->be->invalid_rep, s, msg, msg->msg_state, s->fe);

	/*
	 * 1: get the status code
	 */
	n = rep->buf->p[msg->sl.st.c] - '0';
	if (n < 1 || n > 5)
		n = 0;
	/* when the client triggers a 4xx from the server, it's most often due
	 * to a missing object or permission. These events should be tracked
	 * because if they happen often, it may indicate a brute force or a
	 * vulnerability scan.
	 */
	if (n == 4)
		session_inc_http_err_ctr(s);

	if (objt_server(s->target))
		objt_server(s->target)->counters.p.http.rsp[n]++;

	/* check if the response is HTTP/1.1 or above */
	if ((msg->sl.st.v_l == 8) &&
	    ((rep->buf->p[5] > '1') ||
	     ((rep->buf->p[5] == '1') && (rep->buf->p[7] >= '1'))))
		msg->flags |= HTTP_MSGF_VER_11;

	/* "connection" has not been parsed yet */
	txn->flags &= ~(TX_HDR_CONN_PRS|TX_HDR_CONN_CLO|TX_HDR_CONN_KAL|TX_HDR_CONN_UPG|TX_CON_CLO_SET|TX_CON_KAL_SET);

	/* transfer length unknown*/
	msg->flags &= ~HTTP_MSGF_XFER_LEN;

	txn->status = strl2ui(rep->buf->p + msg->sl.st.c, msg->sl.st.c_l);

	/* Adjust server's health based on status code. Note: status codes 501
	 * and 505 are triggered on demand by client request, so we must not
	 * count them as server failures.
	 */
	if (objt_server(s->target)) {
		if (txn->status >= 100 && (txn->status < 500 || txn->status == 501 || txn->status == 505))
			health_adjust(objt_server(s->target), HANA_STATUS_HTTP_OK);
		else
			health_adjust(objt_server(s->target), HANA_STATUS_HTTP_STS);
	}

	/*
	 * 2: check for cacheability.
	 */

	switch (txn->status) {
	case 200:
	case 203:
	case 206:
	case 300:
	case 301:
	case 410:
		/* RFC2616 @13.4:
		 *   "A response received with a status code of
		 *    200, 203, 206, 300, 301 or 410 MAY be stored
		 *    by a cache (...) unless a cache-control
		 *    directive prohibits caching."
		 *
		 * RFC2616 @9.5: POST method :
		 *   "Responses to this method are not cacheable,
		 *    unless the response includes appropriate
		 *    Cache-Control or Expires header fields."
		 */
		if (likely(txn->meth != HTTP_METH_POST) &&
		    ((s->be->options & PR_O_CHK_CACHE) || (s->be->ck_opts & PR_CK_NOC)))
			txn->flags |= TX_CACHEABLE | TX_CACHE_COOK;
		break;
	default:
		break;
	}

	/*
	 * 3: we may need to capture headers
	 */
	s->logs.logwait &= ~LW_RESP;
	if (unlikely((s->logs.logwait & LW_RSPHDR) && txn->rsp.cap))
		capture_headers(rep->buf->p, &txn->hdr_idx,
				txn->rsp.cap, s->fe->rsp_cap);

	/* 4: determine the transfer-length.
	 * According to RFC2616 #4.4, amended by the HTTPbis working group,
	 * the presence of a message-body in a RESPONSE and its transfer length
	 * must be determined that way :
	 *
	 *   All responses to the HEAD request method MUST NOT include a
	 *   message-body, even though the presence of entity-header fields
	 *   might lead one to believe they do.  All 1xx (informational), 204
	 *   (No Content), and 304 (Not Modified) responses MUST NOT include a
	 *   message-body.  All other responses do include a message-body,
	 *   although it MAY be of zero length.
	 *
	 *   1. Any response which "MUST NOT" include a message-body (such as the
	 *      1xx, 204 and 304 responses and any response to a HEAD request) is
	 *      always terminated by the first empty line after the header fields,
	 *      regardless of the entity-header fields present in the message.
	 *
	 *   2. If a Transfer-Encoding header field (Section 9.7) is present and
	 *      the "chunked" transfer-coding (Section 6.2) is used, the
	 *      transfer-length is defined by the use of this transfer-coding.
	 *      If a Transfer-Encoding header field is present and the "chunked"
	 *      transfer-coding is not present, the transfer-length is defined by
	 *      the sender closing the connection.
	 *
	 *   3. If a Content-Length header field is present, its decimal value in
	 *      OCTETs represents both the entity-length and the transfer-length.
	 *      If a message is received with both a Transfer-Encoding header
	 *      field and a Content-Length header field, the latter MUST be ignored.
	 *
	 *   4. If the message uses the media type "multipart/byteranges", and
	 *      the transfer-length is not otherwise specified, then this self-
	 *      delimiting media type defines the transfer-length.  This media
	 *      type MUST NOT be used unless the sender knows that the recipient
	 *      can parse it; the presence in a request of a Range header with
	 *      multiple byte-range specifiers from a 1.1 client implies that the
	 *      client can parse multipart/byteranges responses.
	 *
	 *   5. By the server closing the connection.
	 */

	/* Skip parsing if no content length is possible. The response flags
	 * remain 0 as well as the chunk_len, which may or may not mirror
	 * the real header value, and we note that we know the response's length.
	 * FIXME: should we parse anyway and return an error on chunked encoding ?
	 */
	if (txn->meth == HTTP_METH_HEAD ||
	    (txn->status >= 100 && txn->status < 200) ||
	    txn->status == 204 || txn->status == 304) {
		msg->flags |= HTTP_MSGF_XFER_LEN;
		s->comp_algo = NULL;
		goto skip_content_length;
	}

	use_close_only = 0;
	ctx.idx = 0;
	while ((msg->flags & HTTP_MSGF_VER_11) &&
	       http_find_header2("Transfer-Encoding", 17, rep->buf->p, &txn->hdr_idx, &ctx)) {
		if (ctx.vlen == 7 && strncasecmp(ctx.line + ctx.val, "chunked", 7) == 0)
			msg->flags |= (HTTP_MSGF_TE_CHNK | HTTP_MSGF_XFER_LEN);
		else if (msg->flags & HTTP_MSGF_TE_CHNK) {
			/* bad transfer-encoding (chunked followed by something else) */
			use_close_only = 1;
			msg->flags &= ~(HTTP_MSGF_TE_CHNK | HTTP_MSGF_XFER_LEN);
			break;
		}
	}

	/* FIXME: below we should remove the content-length header(s) in case of chunked encoding */
	ctx.idx = 0;
	while (!(msg->flags & HTTP_MSGF_TE_CHNK) && !use_close_only &&
	       http_find_header2("Content-Length", 14, rep->buf->p, &txn->hdr_idx, &ctx)) {
		signed long long cl;

		if (!ctx.vlen) {
			msg->err_pos = ctx.line + ctx.val - rep->buf->p;
			goto hdr_response_bad;
		}

		if (strl2llrc(ctx.line + ctx.val, ctx.vlen, &cl)) {
			msg->err_pos = ctx.line + ctx.val - rep->buf->p;
			goto hdr_response_bad; /* parse failure */
		}

		if (cl < 0) {
			msg->err_pos = ctx.line + ctx.val - rep->buf->p;
			goto hdr_response_bad;
		}

		if ((msg->flags & HTTP_MSGF_CNT_LEN) && (msg->chunk_len != cl)) {
			msg->err_pos = ctx.line + ctx.val - rep->buf->p;
			goto hdr_response_bad; /* already specified, was different */
		}

		msg->flags |= HTTP_MSGF_CNT_LEN | HTTP_MSGF_XFER_LEN;
		msg->body_len = msg->chunk_len = cl;
	}

	if (s->fe->comp || s->be->comp)
		select_compression_response_header(s, rep->buf);

	/* FIXME: we should also implement the multipart/byterange method.
	 * For now on, we resort to close mode in this case (unknown length).
	 */
skip_content_length:

	/* end of job, return OK */
	rep->analysers &= ~an_bit;
	rep->analyse_exp = TICK_ETERNITY;
	channel_auto_close(rep);
	return 1;
}