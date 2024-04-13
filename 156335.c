sshpam_thread_conv(int n,
	 const struct pam_message **msg,
	 struct pam_response **resp,
	 void *data)
{
	Buffer buffer;
	struct pam_ctxt *ctxt;
	int i;

	ctxt = data;
	if (n <= 0 || n > PAM_MAX_NUM_MSG)
		return (PAM_CONV_ERR);
	*resp = xmalloc(n * sizeof **resp);
	buffer_init(&buffer);
	for (i = 0; i < n; ++i) {
		resp[i]->resp_retcode = 0;
		resp[i]->resp = NULL;
		switch (PAM_MSG_MEMBER(msg, i, msg_style)) {
		case PAM_PROMPT_ECHO_OFF:
			buffer_put_cstring(&buffer, PAM_MSG_MEMBER(msg, i, msg));
			ssh_msg_send(ctxt->pam_csock, 
			    PAM_MSG_MEMBER(msg, i, msg_style), &buffer);
			ssh_msg_recv(ctxt->pam_csock, &buffer);
			if (buffer_get_char(&buffer) != PAM_AUTHTOK)
				goto fail;
			resp[i]->resp = buffer_get_string(&buffer, NULL);
			break;
		case PAM_PROMPT_ECHO_ON:
			buffer_put_cstring(&buffer, PAM_MSG_MEMBER(msg, i, msg));
			ssh_msg_send(ctxt->pam_csock, 
			    PAM_MSG_MEMBER(msg, i, msg_style), &buffer);
			ssh_msg_recv(ctxt->pam_csock, &buffer);
			if (buffer_get_char(&buffer) != PAM_AUTHTOK)
				goto fail;
			resp[i]->resp = buffer_get_string(&buffer, NULL);
			break;
		case PAM_ERROR_MSG:
			buffer_put_cstring(&buffer, PAM_MSG_MEMBER(msg, i, msg));
			ssh_msg_send(ctxt->pam_csock, 
			    PAM_MSG_MEMBER(msg, i, msg_style), &buffer);
			break;
		case PAM_TEXT_INFO:
			buffer_put_cstring(&buffer, PAM_MSG_MEMBER(msg, i, msg));
			ssh_msg_send(ctxt->pam_csock, 
			    PAM_MSG_MEMBER(msg, i, msg_style), &buffer);
			break;
		default:
			goto fail;
		}
		buffer_clear(&buffer);
	}
	buffer_free(&buffer);
	return (PAM_SUCCESS);
 fail:
	while (i)
		xfree(resp[--i]);
	xfree(*resp);
	*resp = NULL;
	buffer_free(&buffer);
	return (PAM_CONV_ERR);
}