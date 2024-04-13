smp_fetch_base32(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                 const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	struct hdr_ctx ctx;
	unsigned int hash = 0;
	char *ptr, *beg, *end;
	int len;

	CHECK_HTTP_MESSAGE_FIRST();

	ctx.idx = 0;
	if (http_find_header2("Host", 4, txn->req.chn->buf->p + txn->req.sol, &txn->hdr_idx, &ctx)) {
		/* OK we have the header value in ctx.line+ctx.val for ctx.vlen bytes */
		ptr = ctx.line + ctx.val;
		len = ctx.vlen;
		while (len--)
			hash = *(ptr++) + (hash << 6) + (hash << 16) - hash;
	}

	/* now retrieve the path */
	end = txn->req.chn->buf->p + txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l;
	beg = http_get_path(txn);
	if (!beg)
		beg = end;

	for (ptr = beg; ptr < end && *ptr != '?'; ptr++);

	if (beg < ptr && *beg == '/') {
		while (beg < ptr)
			hash = *(beg++) + (hash << 6) + (hash << 16) - hash;
	}
	hash = full_hash(hash);

	smp->type = SMP_T_UINT;
	smp->data.uint = hash;
	smp->flags = SMP_F_VOL_1ST;
	return 1;
}