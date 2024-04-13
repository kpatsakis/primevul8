acl_fetch_cookie_cnt(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                     const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	struct hdr_idx *idx = &txn->hdr_idx;
	struct hdr_ctx ctx;
	const struct http_msg *msg;
	const char *hdr_name;
	int hdr_name_len;
	int cnt;
	char *val_beg, *val_end;
	char *sol;

	if (!args || args->type != ARGT_STR)
		return 0;

	CHECK_HTTP_MESSAGE_FIRST();

	if ((opt & SMP_OPT_DIR) == SMP_OPT_DIR_REQ) {
		msg = &txn->req;
		hdr_name = "Cookie";
		hdr_name_len = 6;
	} else {
		msg = &txn->rsp;
		hdr_name = "Set-Cookie";
		hdr_name_len = 10;
	}

	sol = msg->chn->buf->p;
	val_end = val_beg = NULL;
	ctx.idx = 0;
	cnt = 0;

	while (1) {
		/* Note: val_beg == NULL every time we need to fetch a new header */
		if (!val_beg) {
			if (!http_find_header2(hdr_name, hdr_name_len, sol, idx, &ctx))
				break;

			if (ctx.vlen < args->data.str.len + 1)
				continue;

			val_beg = ctx.line + ctx.val;
			val_end = val_beg + ctx.vlen;
		}

		smp->type = SMP_T_CSTR;
		while ((val_beg = extract_cookie_value(val_beg, val_end,
						       args->data.str.str, args->data.str.len,
						       (opt & SMP_OPT_DIR) == SMP_OPT_DIR_REQ,
						       &smp->data.str.str,
						       &smp->data.str.len))) {
			cnt++;
		}
	}

	smp->data.uint = cnt;
	smp->flags |= SMP_F_VOL_HDR;
	return 1;
}