smp_fetch_hdr_cnt(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                  const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	struct hdr_idx *idx = &txn->hdr_idx;
	struct hdr_ctx ctx;
	const struct http_msg *msg = ((opt & SMP_OPT_DIR) == SMP_OPT_DIR_REQ) ? &txn->req : &txn->rsp;
	int cnt;

	if (!args || args->type != ARGT_STR)
		return 0;

	CHECK_HTTP_MESSAGE_FIRST();

	ctx.idx = 0;
	cnt = 0;
	while (http_find_header2(args->data.str.str, args->data.str.len, msg->chn->buf->p, idx, &ctx))
		cnt++;

	smp->type = SMP_T_UINT;
	smp->data.uint = cnt;
	smp->flags = SMP_F_VOL_HDR;
	return 1;
}