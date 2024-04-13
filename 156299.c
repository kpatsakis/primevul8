mm_answer_pam_respond(int socket, Buffer *m)
{
	char **resp;
	u_int num;
	int i, ret;

	debug3("%s", __func__);
	sshpam_authok = NULL;
	num = buffer_get_int(m);
	if (num > 0) {
		resp = xmalloc(num * sizeof(char *));
		for (i = 0; i < num; ++i)
			resp[i] = buffer_get_string(m, NULL);
		ret = (sshpam_device.respond)(sshpam_ctxt, num, resp);
		for (i = 0; i < num; ++i)
			xfree(resp[i]);
		xfree(resp);
	} else {
		ret = (sshpam_device.respond)(sshpam_ctxt, num, NULL);
	}
	buffer_clear(m);
	buffer_put_int(m, ret);
	mm_request_send(socket, MONITOR_ANS_PAM_RESPOND, m);
	auth_method = "keyboard-interactive/pam";
	if (ret == 0)
		sshpam_authok = sshpam_ctxt;
	return (0);
}