void http_parse_connection_header(struct http_txn *txn, struct http_msg *msg, int to_del)
{
	struct hdr_ctx ctx;
	const char *hdr_val = "Connection";
	int hdr_len = 10;

	if (txn->flags & TX_HDR_CONN_PRS)
		return;

	if (unlikely(txn->flags & TX_USE_PX_CONN)) {
		hdr_val = "Proxy-Connection";
		hdr_len = 16;
	}

	ctx.idx = 0;
	txn->flags &= ~(TX_CON_KAL_SET|TX_CON_CLO_SET);
	while (http_find_header2(hdr_val, hdr_len, msg->chn->buf->p, &txn->hdr_idx, &ctx)) {
		if (ctx.vlen >= 10 && word_match(ctx.line + ctx.val, ctx.vlen, "keep-alive", 10)) {
			txn->flags |= TX_HDR_CONN_KAL;
			if (to_del & 2)
				http_remove_header2(msg, &txn->hdr_idx, &ctx);
			else
				txn->flags |= TX_CON_KAL_SET;
		}
		else if (ctx.vlen >= 5 && word_match(ctx.line + ctx.val, ctx.vlen, "close", 5)) {
			txn->flags |= TX_HDR_CONN_CLO;
			if (to_del & 1)
				http_remove_header2(msg, &txn->hdr_idx, &ctx);
			else
				txn->flags |= TX_CON_CLO_SET;
		}
		else if (ctx.vlen >= 7 && word_match(ctx.line + ctx.val, ctx.vlen, "upgrade", 7)) {
			txn->flags |= TX_HDR_CONN_UPG;
		}
	}

	txn->flags |= TX_HDR_CONN_PRS;
	return;
}