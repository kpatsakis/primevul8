smp_fetch_url(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
              const struct arg *args, struct sample *smp)
{
	struct http_txn *txn = l7;

	CHECK_HTTP_MESSAGE_FIRST();

	smp->type = SMP_T_CSTR;
	smp->data.str.len = txn->req.sl.rq.u_l;
	smp->data.str.str = txn->req.chn->buf->p + txn->req.sl.rq.u;
	smp->flags = SMP_F_VOL_1ST;
	return 1;
}