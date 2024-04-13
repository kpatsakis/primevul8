acl_fetch_stver(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	char *ptr;
	int len;

	CHECK_HTTP_MESSAGE_FIRST();

	if (txn->rsp.msg_state < HTTP_MSG_BODY)
		return 0;

	len = txn->rsp.sl.st.v_l;
	ptr = txn->rsp.chn->buf->p;

	while ((len-- > 0) && (*ptr++ != '/'));
	if (len <= 0)
		return 0;

	smp->type = SMP_T_CSTR;
	smp->data.str.str = ptr;
	smp->data.str.len = len;

	smp->flags = SMP_F_VOL_1ST;
	return 1;
}