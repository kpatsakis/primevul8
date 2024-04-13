int http_process_request_body(struct session *s, struct channel *req, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &s->txn.req;
	long long limit = s->be->url_param_post_limit;

	/* We have to parse the HTTP request body to find any required data.
	 * "balance url_param check_post" should have been the only way to get
	 * into this. We were brought here after HTTP header analysis, so all
	 * related structures are ready.
	 */

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))
		goto missing_data;

	if (msg->msg_state < HTTP_MSG_100_SENT) {
		/* If we have HTTP/1.1 and Expect: 100-continue, then we must
		 * send an HTTP/1.1 100 Continue intermediate response.
		 */
		if (msg->flags & HTTP_MSGF_VER_11) {
			struct hdr_ctx ctx;
			ctx.idx = 0;
			/* Expect is allowed in 1.1, look for it */
			if (http_find_header2("Expect", 6, req->buf->p, &txn->hdr_idx, &ctx) &&
			    unlikely(ctx.vlen == 12 && strncasecmp(ctx.line+ctx.val, "100-continue", 12) == 0)) {
				bo_inject(s->rep, http_100_chunk.str, http_100_chunk.len);
			}
		}
		msg->msg_state = HTTP_MSG_100_SENT;
	}

	if (msg->msg_state < HTTP_MSG_CHUNK_SIZE) {
		/* we have msg->sov which points to the first byte of message body.
		 * req->buf->p still points to the beginning of the message and msg->sol
		 * is still null. We must save the body in msg->next because it
		 * survives buffer re-alignments.
		 */
		msg->next = msg->sov;

		if (msg->flags & HTTP_MSGF_TE_CHNK)
			msg->msg_state = HTTP_MSG_CHUNK_SIZE;
		else
			msg->msg_state = HTTP_MSG_DATA;
	}

	if (msg->msg_state == HTTP_MSG_CHUNK_SIZE) {
		/* read the chunk size and assign it to ->chunk_len, then
		 * set ->sov and ->next to point to the body and switch to DATA or
		 * TRAILERS state.
		 */
		int ret = http_parse_chunk_size(msg);

		if (!ret)
			goto missing_data;
		else if (ret < 0) {
			session_inc_http_err_ctr(s);
			goto return_bad_req;
		}
	}

	/* Now we're in HTTP_MSG_DATA or HTTP_MSG_TRAILERS state.
	 * We have the first data byte is in msg->sov. We're waiting for at
	 * least <url_param_post_limit> bytes after msg->sov.
	 */

	if (msg->body_len < limit)
		limit = msg->body_len;

	if (req->buf->i - msg->sov >= limit)    /* we have enough bytes now */
		goto http_end;

 missing_data:
	/* we get here if we need to wait for more data */
	if (buffer_full(req->buf, global.tune.maxrewrite)) {
		session_inc_http_err_ctr(s);
		goto return_bad_req;
	}

	if ((req->flags & CF_READ_TIMEOUT) || tick_is_expired(req->analyse_exp, now_ms)) {
		txn->status = 408;
		stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_408));

		if (!(s->flags & SN_ERR_MASK))
			s->flags |= SN_ERR_CLITO;
		if (!(s->flags & SN_FINST_MASK))
			s->flags |= SN_FINST_D;
		goto return_err_msg;
	}

	/* we get here if we need to wait for more data */
	if (!(req->flags & (CF_SHUTR | CF_READ_ERROR)) && !buffer_full(req->buf, global.tune.maxrewrite)) {
		/* Not enough data. We'll re-use the http-request
		 * timeout here. Ideally, we should set the timeout
		 * relative to the accept() date. We just set the
		 * request timeout once at the beginning of the
		 * request.
		 */
		channel_dont_connect(req);
		if (!tick_isset(req->analyse_exp))
			req->analyse_exp = tick_add_ifset(now_ms, s->be->timeout.httpreq);
		return 0;
	}

 http_end:
	/* The situation will not evolve, so let's give up on the analysis. */
	s->logs.tv_request = now;  /* update the request timer to reflect full request */
	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;
	return 1;

 return_bad_req: /* let's centralize all bad requests */
	txn->req.msg_state = HTTP_MSG_ERROR;
	txn->status = 400;
	stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

 return_err_msg:
	req->analysers = 0;
	s->fe->fe_counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;
	return 0;
}