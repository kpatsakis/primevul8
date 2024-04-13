smp_fetch_url_param(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                    const struct arg *args, struct sample *smp)
{
	char delim = '?';
	struct http_txn *txn = l7;
	struct http_msg *msg = &txn->req;

	if (!args || args[0].type != ARGT_STR ||
	    (args[1].type && args[1].type != ARGT_STR))
		return 0;

	CHECK_HTTP_MESSAGE_FIRST();

	if (args[1].type)
		delim = *args[1].data.str.str;

	if (!find_url_param_value(msg->chn->buf->p + msg->sl.rq.u, msg->sl.rq.u_l,
                                 args->data.str.str, args->data.str.len,
                                 &smp->data.str.str, &smp->data.str.len,
                                 delim))
		return 0;

	smp->type = SMP_T_CSTR;
	smp->flags = SMP_F_VOL_1ST;
	return 1;
}