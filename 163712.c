acl_fetch_http_first_req(struct proxy *px, struct session *s, void *l7, unsigned int opt,
                         const struct arg *args, struct sample *smp)
{
	if (!s)
		return 0;

	smp->type = SMP_T_BOOL;
	smp->data.uint = !(s->txn.flags & TX_NOT_FIRST);
	return 1;
}