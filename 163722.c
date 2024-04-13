acl_fetch_stcode(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                 const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;
	char *ptr;
	int len;

	CHECK_HTTP_MESSAGE_FIRST();

	if (txn->rsp.msg_state < HTTP_MSG_BODY)
		return 0;

	len = txn->rsp.sl.st.c_l;
	ptr = txn->rsp.chn->buf->p + txn->rsp.sl.st.c;

	smp->type = SMP_T_UINT;
	smp->data.uint = __strl2ui(ptr, len);
	smp->flags = SMP_F_VOL_1ST;
	return 1;
}