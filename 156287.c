mm_answer_pam_query(int socket, Buffer *m)
{
	char *name, *info, **prompts;
	u_int num, *echo_on;
	int i, ret;

	debug3("%s", __func__);
	sshpam_authok = NULL;
	ret = (sshpam_device.query)(sshpam_ctxt, &name, &info, &num, &prompts, &echo_on);
	if (ret == 0 && num == 0)
		sshpam_authok = sshpam_ctxt;
	if (num > 1 || name == NULL || info == NULL)
		ret = -1;
	buffer_clear(m);
	buffer_put_int(m, ret);
	buffer_put_cstring(m, name);
	xfree(name);
	buffer_put_cstring(m, info);
	xfree(info);
	buffer_put_int(m, num);
	for (i = 0; i < num; ++i) {
		buffer_put_cstring(m, prompts[i]);
		xfree(prompts[i]);
		buffer_put_int(m, echo_on[i]);
	}
	if (prompts != NULL)
		xfree(prompts);
	if (echo_on != NULL)
		xfree(echo_on);
	mm_request_send(socket, MONITOR_ANS_PAM_QUERY, m);
	return (0);
}