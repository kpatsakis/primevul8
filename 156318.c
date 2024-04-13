pam_chauthtok_conv(int n,
	 const struct pam_message **msg,
	 struct pam_response **resp,
	 void *data)
{
	char input[PAM_MAX_MSG_SIZE];
	int i;

	if (n <= 0 || n > PAM_MAX_NUM_MSG)
		return (PAM_CONV_ERR);
	*resp = xmalloc(n * sizeof **resp);
	for (i = 0; i < n; ++i) {
		switch (PAM_MSG_MEMBER(msg, i, msg_style)) {
		case PAM_PROMPT_ECHO_OFF:
			resp[i]->resp =
			    read_passphrase(PAM_MSG_MEMBER(msg, i, msg), 
			    RP_ALLOW_STDIN);
			resp[i]->resp_retcode = PAM_SUCCESS;
			break;
		case PAM_PROMPT_ECHO_ON:
			fputs(PAM_MSG_MEMBER(msg, i, msg), stderr);
			fgets(input, sizeof input, stdin);
			resp[i]->resp = xstrdup(input);
			resp[i]->resp_retcode = PAM_SUCCESS;
			break;
		case PAM_ERROR_MSG:
		case PAM_TEXT_INFO:
			fputs(PAM_MSG_MEMBER(msg, i, msg), stderr);
			resp[i]->resp_retcode = PAM_SUCCESS;
			break;
		default:
			goto fail;
		}
	}
	return (PAM_SUCCESS);
 fail:
	while (i)
		xfree(resp[--i]);
	xfree(*resp);
	*resp = NULL;
	return (PAM_CONV_ERR);
}