mm_answer_bsdauthquery(int socket, Buffer *m)
{
	char *name, *infotxt;
	u_int numprompts;
	u_int *echo_on;
	char **prompts;
	u_int success;

	success = bsdauth_query(authctxt, &name, &infotxt, &numprompts,
	    &prompts, &echo_on) < 0 ? 0 : 1;

	buffer_clear(m);
	buffer_put_int(m, success);
	if (success)
		buffer_put_cstring(m, prompts[0]);

	debug3("%s: sending challenge success: %u", __func__, success);
	mm_request_send(socket, MONITOR_ANS_BSDAUTHQUERY, m);

	if (success) {
		xfree(name);
		xfree(infotxt);
		xfree(prompts);
		xfree(echo_on);
	}

	return (0);
}