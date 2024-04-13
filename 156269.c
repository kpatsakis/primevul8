mm_answer_authpassword(int socket, Buffer *m)
{
	static int call_count;
	char *passwd;
	int authenticated;
	u_int plen;

	passwd = buffer_get_string(m, &plen);
	/* Only authenticate if the context is valid */
	authenticated = options.password_authentication &&
	    auth_password(authctxt, passwd) && authctxt->valid;
	memset(passwd, 0, strlen(passwd));
	xfree(passwd);

	buffer_clear(m);
	buffer_put_int(m, authenticated);

	debug3("%s: sending result %d", __func__, authenticated);
	mm_request_send(socket, MONITOR_ANS_AUTHPASSWORD, m);

	call_count++;
	if (plen == 0 && call_count == 1)
		auth_method = "none";
	else
		auth_method = "password";

	/* Causes monitor loop to terminate if authenticated */
	return (authenticated);
}