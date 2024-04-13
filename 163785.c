smp_fetch_base(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
               const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	char *ptr, *end, *beg;
	struct hdr_ctx ctx;

	CHECK_HTTP_MESSAGE_FIRST();

	ctx.idx = 0;
	if (!http_find_header2("Host", 4, txn->req.chn->buf->p + txn->req.sol, &txn->hdr_idx, &ctx) ||
	    !ctx.vlen)
		return smp_fetch_path(px, l4, l7, opt, args, smp);

	/* OK we have the header value in ctx.line+ctx.val for ctx.vlen bytes */
	memcpy(trash.str, ctx.line + ctx.val, ctx.vlen);
	smp->type = SMP_T_STR;
	smp->data.str.str = trash.str;
	smp->data.str.len = ctx.vlen;

	/* now retrieve the path */
	end = txn->req.chn->buf->p + txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l;
	beg = http_get_path(txn);
	if (!beg)
		beg = end;

	for (ptr = beg; ptr < end && *ptr != '?'; ptr++);

	if (beg < ptr && *beg == '/') {
		memcpy(smp->data.str.str + smp->data.str.len, beg, ptr - beg);
		smp->data.str.len += ptr - beg;
	}

	smp->flags = SMP_F_VOL_1ST;
	return 1;
}