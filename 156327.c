mm_auth_krb4(Authctxt *authctxt, void *_auth, char **client, void *_reply)
{
	KTEXT auth, reply;
 	Buffer m;
	u_int rlen;
	int success = 0;
	char *p;

	debug3("%s entering", __func__);
	auth = _auth;
	reply = _reply;

	buffer_init(&m);
	buffer_put_string(&m, auth->dat, auth->length);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_KRB4, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_KRB4, &m);

	success = buffer_get_int(&m);
	if (success) {
		*client = buffer_get_string(&m, NULL);
		p = buffer_get_string(&m, &rlen);
		if (rlen >= MAX_KTXT_LEN)
			fatal("%s: reply from monitor too large", __func__);
		reply->length = rlen;
		memcpy(reply->dat, p, rlen);
		memset(p, 0, rlen);
		xfree(p);
	}
	buffer_free(&m);
	return (success);
}