int select_compression_response_header(struct session *s, struct buffer *res)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->rsp;
	struct hdr_ctx ctx;
	struct comp_type *comp_type;

	/* no common compression algorithm was found in request header */
	if (s->comp_algo == NULL)
		goto fail;

	/* HTTP < 1.1 should not be compressed */
	if (!(msg->flags & HTTP_MSGF_VER_11))
		goto fail;

	/* 200 only */
	if (txn->status != 200)
		goto fail;

	/* Content-Length is null */
	if (!(msg->flags & HTTP_MSGF_TE_CHNK) && msg->body_len == 0)
		goto fail;

	/* content is already compressed */
	ctx.idx = 0;
	if (http_find_header2("Content-Encoding", 16, res->p, &txn->hdr_idx, &ctx))
		goto fail;

	/* no compression when Cache-Control: no-transform is present in the message */
	ctx.idx = 0;
	while (http_find_header2("Cache-Control", 13, res->p, &txn->hdr_idx, &ctx)) {
		if (word_match(ctx.line + ctx.val, ctx.vlen, "no-transform", 12))
			goto fail;
	}

	comp_type = NULL;

	/* we don't want to compress multipart content-types, nor content-types that are
	 * not listed in the "compression type" directive if any. If no content-type was
	 * found but configuration requires one, we don't compress either. Backend has
	 * the priority.
	 */
	ctx.idx = 0;
	if (http_find_header2("Content-Type", 12, res->p, &txn->hdr_idx, &ctx)) {
		if (ctx.vlen >= 9 && strncasecmp("multipart", ctx.line+ctx.val, 9) == 0)
			goto fail;

		if ((s->be->comp && (comp_type = s->be->comp->types)) ||
		    (s->fe->comp && (comp_type = s->fe->comp->types))) {
			for (; comp_type; comp_type = comp_type->next) {
				if (ctx.vlen >= comp_type->name_len &&
				    strncasecmp(ctx.line+ctx.val, comp_type->name, comp_type->name_len) == 0)
					/* this Content-Type should be compressed */
					break;
			}
			/* this Content-Type should not be compressed */
			if (comp_type == NULL)
				goto fail;
		}
	}
	else { /* no content-type header */
		if ((s->be->comp && s->be->comp->types) || (s->fe->comp && s->fe->comp->types))
			goto fail; /* a content-type was required */
	}

	/* limit compression rate */
	if (global.comp_rate_lim > 0)
		if (read_freq_ctr(&global.comp_bps_in) > global.comp_rate_lim)
			goto fail;

	/* limit cpu usage */
	if (idle_pct < compress_min_idle)
		goto fail;

	/* initialize compression */
	if (s->comp_algo->init(&s->comp_ctx, global.tune.comp_maxlevel) < 0)
		goto fail;

	s->flags |= SN_COMP_READY;

	/* remove Content-Length header */
	ctx.idx = 0;
	if ((msg->flags & HTTP_MSGF_CNT_LEN) && http_find_header2("Content-Length", 14, res->p, &txn->hdr_idx, &ctx))
		http_remove_header2(msg, &txn->hdr_idx, &ctx);

	/* add Transfer-Encoding header */
	if (!(msg->flags & HTTP_MSGF_TE_CHNK))
		http_header_add_tail2(&txn->rsp, &txn->hdr_idx, "Transfer-Encoding: chunked", 26);

	/*
	 * Add Content-Encoding header when it's not identity encoding.
         * RFC 2616 : Identity encoding: This content-coding is used only in the
	 * Accept-Encoding header, and SHOULD NOT be used in the Content-Encoding
	 * header.
	 */
	if (s->comp_algo->add_data != identity_add_data) {
		trash.len = 18;
		memcpy(trash.str, "Content-Encoding: ", trash.len);
		memcpy(trash.str + trash.len, s->comp_algo->name, s->comp_algo->name_len);
		trash.len += s->comp_algo->name_len;
		trash.str[trash.len] = '\0';
		http_header_add_tail2(&txn->rsp, &txn->hdr_idx, trash.str, trash.len);
	}
	return 1;

fail:
	s->comp_algo = NULL;
	return 0;
}