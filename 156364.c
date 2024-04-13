mm_answer_sesskey(int socket, Buffer *m)
{
	BIGNUM *p;
	int rsafail;

	/* Turn off permissions */
	monitor_permit(mon_dispatch, MONITOR_REQ_SESSKEY, 1);

	if ((p = BN_new()) == NULL)
		fatal("%s: BN_new", __func__);

	buffer_get_bignum2(m, p);

	rsafail = ssh1_session_key(p);

	buffer_clear(m);
	buffer_put_int(m, rsafail);
	buffer_put_bignum2(m, p);

	BN_clear_free(p);

	mm_request_send(socket, MONITOR_ANS_SESSKEY, m);

	/* Turn on permissions for sessid passing */
	monitor_permit(mon_dispatch, MONITOR_REQ_SESSID, 1);

	return (0);
}