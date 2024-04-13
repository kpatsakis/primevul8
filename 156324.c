mm_answer_krb5(int socket, Buffer *m)
{
	krb5_data tkt, reply;
	char *client_user;
	u_int len;
	int success;

	/* use temporary var to avoid size issues on 64bit arch */
	tkt.data = buffer_get_string(m, &len);
	tkt.length = len;

	success = options.kerberos_authentication &&
	    authctxt->valid &&
	    auth_krb5(authctxt, &tkt, &client_user, &reply);

	if (tkt.length)
		xfree(tkt.data);

	buffer_clear(m);
	buffer_put_int(m, success);

	if (success) {
		buffer_put_cstring(m, client_user);
		buffer_put_string(m, reply.data, reply.length);
		if (client_user)
			xfree(client_user);
		if (reply.length)
			xfree(reply.data);
	}
	mm_request_send(socket, MONITOR_ANS_KRB5, m);

	return success;
}