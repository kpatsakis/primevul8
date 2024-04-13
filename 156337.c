mm_skey_query(void *ctx, char **name, char **infotxt,
   u_int *numprompts, char ***prompts, u_int **echo_on)
{
	Buffer m;
	int len;
	u_int success;
	char *p, *challenge;

	debug3("%s: entering", __func__);

	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_SKEYQUERY, &m);

	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_SKEYQUERY,
	    &m);
	success = buffer_get_int(&m);
	if (success == 0) {
		debug3("%s: no challenge", __func__);
		buffer_free(&m);
		return (-1);
	}

	/* Get the challenge, and format the response */
	challenge  = buffer_get_string(&m, NULL);
	buffer_free(&m);

	debug3("%s: received challenge: %s", __func__, challenge);

	mm_chall_setup(name, infotxt, numprompts, prompts, echo_on);

	len = strlen(challenge) + strlen(SKEY_PROMPT) + 1;
	p = xmalloc(len);
	strlcpy(p, challenge, len);
	strlcat(p, SKEY_PROMPT, len);
	(*prompts)[0] = p;
	xfree(challenge);

	return (0);
}