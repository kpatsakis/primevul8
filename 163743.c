void http_change_connection_header(struct http_txn *txn, struct http_msg *msg, int wanted)
{
	struct hdr_ctx ctx;
	const char *hdr_val = "Connection";
	int hdr_len = 10;

	ctx.idx = 0;


	if (unlikely(txn->flags & TX_USE_PX_CONN)) {
		hdr_val = "Proxy-Connection";
		hdr_len = 16;
	}

	txn->flags &= ~(TX_CON_CLO_SET | TX_CON_KAL_SET);
	while (http_find_header2(hdr_val, hdr_len, msg->chn->buf->p, &txn->hdr_idx, &ctx)) {
		if (ctx.vlen >= 10 && word_match(ctx.line + ctx.val, ctx.vlen, "keep-alive", 10)) {
			if (wanted & TX_CON_KAL_SET)
				txn->flags |= TX_CON_KAL_SET;
			else
				http_remove_header2(msg, &txn->hdr_idx, &ctx);
		}
		else if (ctx.vlen >= 5 && word_match(ctx.line + ctx.val, ctx.vlen, "close", 5)) {
			if (wanted & TX_CON_CLO_SET)
				txn->flags |= TX_CON_CLO_SET;
			else
				http_remove_header2(msg, &txn->hdr_idx, &ctx);
		}
	}

	if (wanted == (txn->flags & (TX_CON_CLO_SET|TX_CON_KAL_SET)))
		return;

	if ((wanted & TX_CON_CLO_SET) && !(txn->flags & TX_CON_CLO_SET)) {
		txn->flags |= TX_CON_CLO_SET;
		hdr_val = "Connection: close";
		hdr_len  = 17;
		if (unlikely(txn->flags & TX_USE_PX_CONN)) {
			hdr_val = "Proxy-Connection: close";
			hdr_len = 23;
		}
		http_header_add_tail2(msg, &txn->hdr_idx, hdr_val, hdr_len);
	}

	if ((wanted & TX_CON_KAL_SET) && !(txn->flags & TX_CON_KAL_SET)) {
		txn->flags |= TX_CON_KAL_SET;
		hdr_val = "Connection: keep-alive";
		hdr_len = 22;
		if (unlikely(txn->flags & TX_USE_PX_CONN)) {
			hdr_val = "Proxy-Connection: keep-alive";
			hdr_len = 28;
		}
		http_header_add_tail2(msg, &txn->hdr_idx, hdr_val, hdr_len);
	}
	return;
}