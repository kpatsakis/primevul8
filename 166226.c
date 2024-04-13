int http_apply_redirect_rule(struct redirect_rule *rule, struct stream *s, struct http_txn *txn)
{
	struct channel *req = &s->req;
	struct channel *res = &s->res;
	struct htx *htx;
	struct htx_sl *sl;
	struct buffer *chunk;
	struct ist status, reason, location;
	unsigned int flags;
	int close = 0; /* Try to keep the connection alive byt default */

	chunk = alloc_trash_chunk();
	if (!chunk) {
		if (!(s->flags & SF_ERR_MASK))
			s->flags |= SF_ERR_RESOURCE;
		goto fail;
	}

	/*
	 * Create the location
	 */
	htx = htxbuf(&req->buf);
	switch(rule->type) {
		case REDIRECT_TYPE_SCHEME: {
			struct http_hdr_ctx ctx;
			struct ist path, host;
			struct http_uri_parser parser;

			host = ist("");
			ctx.blk = NULL;
			if (http_find_header(htx, ist("Host"), &ctx, 0))
				host = ctx.value;

			sl = http_get_stline(htx);
			parser = http_uri_parser_init(htx_sl_req_uri(sl));
			path = http_parse_path(&parser);
			/* build message using path */
			if (isttest(path)) {
				if (rule->flags & REDIRECT_FLAG_DROP_QS) {
					int qs = 0;
					while (qs < path.len) {
						if (*(path.ptr + qs) == '?') {
							path.len = qs;
							break;
						}
						qs++;
					}
				}
			}
			else
				path = ist("/");

			if (rule->rdr_str) { /* this is an old "redirect" rule */
				/* add scheme */
				if (!chunk_memcat(chunk, rule->rdr_str, rule->rdr_len))
					goto fail;
			}
			else {
				/* add scheme with executing log format */
				chunk->data += build_logline(s, chunk->area + chunk->data,
							     chunk->size - chunk->data,
							     &rule->rdr_fmt);
			}
			/* add "://" + host + path */
			if (!chunk_memcat(chunk, "://", 3) ||
			    !chunk_memcat(chunk, host.ptr, host.len) ||
			    !chunk_memcat(chunk, path.ptr, path.len))
				goto fail;

			/* append a slash at the end of the location if needed and missing */
			if (chunk->data && chunk->area[chunk->data - 1] != '/' &&
			    (rule->flags & REDIRECT_FLAG_APPEND_SLASH)) {
				if (chunk->data + 1 >= chunk->size)
					goto fail;
				chunk->area[chunk->data++] = '/';
			}
			break;
		}

		case REDIRECT_TYPE_PREFIX: {
			struct ist path;
			struct http_uri_parser parser;

			sl = http_get_stline(htx);
			parser = http_uri_parser_init(htx_sl_req_uri(sl));
			path = http_parse_path(&parser);
			/* build message using path */
			if (isttest(path)) {
				if (rule->flags & REDIRECT_FLAG_DROP_QS) {
					int qs = 0;
					while (qs < path.len) {
						if (*(path.ptr + qs) == '?') {
							path.len = qs;
							break;
						}
						qs++;
					}
				}
			}
			else
				path = ist("/");

			if (rule->rdr_str) { /* this is an old "redirect" rule */
				/* add prefix. Note that if prefix == "/", we don't want to
				 * add anything, otherwise it makes it hard for the user to
				 * configure a self-redirection.
				 */
				if (rule->rdr_len != 1 || *rule->rdr_str != '/') {
					if (!chunk_memcat(chunk, rule->rdr_str, rule->rdr_len))
						goto fail;
				}
			}
			else {
				/* add prefix with executing log format */
				chunk->data += build_logline(s, chunk->area + chunk->data,
							     chunk->size - chunk->data,
							     &rule->rdr_fmt);
			}

			/* add path */
			if (!chunk_memcat(chunk, path.ptr, path.len))
				goto fail;

			/* append a slash at the end of the location if needed and missing */
			if (chunk->data && chunk->area[chunk->data - 1] != '/' &&
			    (rule->flags & REDIRECT_FLAG_APPEND_SLASH)) {
				if (chunk->data + 1 >= chunk->size)
					goto fail;
				chunk->area[chunk->data++] = '/';
			}
			break;
		}
		case REDIRECT_TYPE_LOCATION:
		default:
			if (rule->rdr_str) { /* this is an old "redirect" rule */
				/* add location */
				if (!chunk_memcat(chunk, rule->rdr_str, rule->rdr_len))
					goto fail;
			}
			else {
				/* add location with executing log format */
				int len = build_logline(s, chunk->area + chunk->data,
				                        chunk->size - chunk->data,
				                        &rule->rdr_fmt);
				if (!len && rule->flags & REDIRECT_FLAG_IGNORE_EMPTY)
					return 2;

				chunk->data += len;
			}
			break;
	}
	location = ist2(chunk->area, chunk->data);

	/*
	 * Create the 30x response
	 */
	switch (rule->code) {
		case 308:
			status = ist("308");
			reason = ist("Permanent Redirect");
			break;
		case 307:
			status = ist("307");
			reason = ist("Temporary Redirect");
			break;
		case 303:
			status = ist("303");
			reason = ist("See Other");
			break;
		case 301:
			status = ist("301");
			reason = ist("Moved Permanently");
			break;
		case 302:
		default:
			status = ist("302");
			reason = ist("Found");
			break;
	}

	if (!(txn->req.flags & HTTP_MSGF_BODYLESS) && txn->req.msg_state != HTTP_MSG_DONE)
		close = 1;

	htx = htx_from_buf(&res->buf);
	/* Trim any possible response */
	channel_htx_truncate(&s->res, htx);
	flags = (HTX_SL_F_IS_RESP|HTX_SL_F_VER_11|HTX_SL_F_XFER_LEN|HTX_SL_F_BODYLESS);
	sl = htx_add_stline(htx, HTX_BLK_RES_SL, flags, ist("HTTP/1.1"), status, reason);
	if (!sl)
		goto fail;
	sl->info.res.status = rule->code;
	s->txn->status = rule->code;

	if (close && !htx_add_header(htx, ist("Connection"), ist("close")))
		goto fail;

	if (!htx_add_header(htx, ist("Content-length"), ist("0")) ||
	    !htx_add_header(htx, ist("Location"), location))
		goto fail;

	if (rule->code == 302 || rule->code == 303 || rule->code == 307) {
		if (!htx_add_header(htx, ist("Cache-Control"), ist("no-cache")))
			goto fail;
	}

	if (rule->cookie_len) {
		if (!htx_add_header(htx, ist("Set-Cookie"), ist2(rule->cookie_str, rule->cookie_len)))
			goto fail;
	}

	if (!htx_add_endof(htx, HTX_BLK_EOH))
		goto fail;

	htx->flags |= HTX_FL_EOM;
	htx_to_buf(htx, &res->buf);
	if (!http_forward_proxy_resp(s, 1))
		goto fail;

	if (rule->flags & REDIRECT_FLAG_FROM_REQ) {
		/* let's log the request time */
		s->logs.tv_request = now;
		req->analysers &= AN_REQ_FLT_END;

		if (s->sess->fe == s->be) /* report it if the request was intercepted by the frontend */
			_HA_ATOMIC_INC(&s->sess->fe->fe_counters.intercepted_req);
	}

	if (!(s->flags & SF_ERR_MASK))
		s->flags |= SF_ERR_LOCAL;
	if (!(s->flags & SF_FINST_MASK))
		s->flags |= ((rule->flags & REDIRECT_FLAG_FROM_REQ) ? SF_FINST_R : SF_FINST_H);

	free_trash_chunk(chunk);
	return 1;

  fail:
	/* If an error occurred, remove the incomplete HTTP response from the
	 * buffer */
	channel_htx_truncate(res, htxbuf(&res->buf));
	free_trash_chunk(chunk);
	return 0;
}