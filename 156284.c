mm_answer_krb4(int socket, Buffer *m)
{
	KTEXT_ST auth, reply;
	char  *client, *p;
	int success;
	u_int alen;

	reply.length = auth.length = 0;
 
	p = buffer_get_string(m, &alen);
	if (alen >=  MAX_KTXT_LEN)
		 fatal("%s: auth too large", __func__);
	memcpy(auth.dat, p, alen);
	auth.length = alen;
	memset(p, 0, alen);
	xfree(p);

	success = options.kerberos_authentication &&
	    authctxt->valid &&
	    auth_krb4(authctxt, &auth, &client, &reply);

	memset(auth.dat, 0, alen);
	buffer_clear(m);
	buffer_put_int(m, success);

	if (success) {
		buffer_put_cstring(m, client);
		buffer_put_string(m, reply.dat, reply.length);
		if (client)
			xfree(client);
		if (reply.length)
			memset(reply.dat, 0, reply.length);
	}

	debug3("%s: sending result %d", __func__, success);
	mm_request_send(socket, MONITOR_ANS_KRB4, m);

	auth_method = "kerberos";

	/* Causes monitor loop to terminate if authenticated */
	return (success);
}