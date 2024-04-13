acl_fetch_rqver(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	char *ptr;
	int len;

	CHECK_HTTP_MESSAGE_FIRST();

	len = txn->req.sl.rq.v_l;
	ptr = txn->req.chn->buf->p + txn->req.sl.rq.v;

	while ((len-- > 0) && (*ptr++ != '/'));
	if (len <= 0)
		return 0;

	smp->type = SMP_T_CSTR;
	smp->data.str.str = ptr;
	smp->data.str.len = len;

	smp->flags = SMP_F_VOL_1ST;
	return 1;
}