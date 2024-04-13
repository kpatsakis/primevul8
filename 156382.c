mm_answer_bsdauthrespond(int socket, Buffer *m)
{
	char *response;
	int authok;

	if (authctxt->as == 0)
		fatal("%s: no bsd auth session", __func__);

	response = buffer_get_string(m, NULL);
	authok = options.challenge_response_authentication &&
	    auth_userresponse(authctxt->as, response, 0);
	authctxt->as = NULL;
	debug3("%s: <%s> = <%d>", __func__, response, authok);
	xfree(response);

	buffer_clear(m);
	buffer_put_int(m, authok);

	debug3("%s: sending authenticated: %d", __func__, authok);
	mm_request_send(socket, MONITOR_ANS_BSDAUTHRESPOND, m);

	auth_method = "bsdauth";

	return (authok != 0);
}