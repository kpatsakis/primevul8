sshpam_query(void *ctx, char **name, char **info,
    u_int *num, char ***prompts, u_int **echo_on)
{
	Buffer buffer;
	struct pam_ctxt *ctxt = ctx;
	size_t plen;
	u_char type;
	char *msg;

	buffer_init(&buffer);
	*name = xstrdup("");
	*info = xstrdup("");
	*prompts = xmalloc(sizeof(char *));
	**prompts = NULL;
	plen = 0;
	*echo_on = xmalloc(sizeof(u_int));
	while (ssh_msg_recv(ctxt->pam_psock, &buffer) == 0) {
		type = buffer_get_char(&buffer);
		msg = buffer_get_string(&buffer, NULL);
		switch (type) {
		case PAM_PROMPT_ECHO_ON:
		case PAM_PROMPT_ECHO_OFF:
			*num = 1;
			**prompts = xrealloc(**prompts, plen + strlen(msg) + 1);
			plen += sprintf(**prompts + plen, "%s", msg);
			**echo_on = (type == PAM_PROMPT_ECHO_ON);
			xfree(msg);
			return (0);
		case PAM_ERROR_MSG:
		case PAM_TEXT_INFO:
			/* accumulate messages */
			**prompts = xrealloc(**prompts, plen + strlen(msg) + 1);
			plen += sprintf(**prompts + plen, "%s", msg);
			xfree(msg);
			break;
		case PAM_NEW_AUTHTOK_REQD:
			sshpam_new_authtok_reqd = 1;
			/* FALLTHROUGH */
		case PAM_SUCCESS:
		case PAM_AUTH_ERR:
			if (**prompts != NULL) {
				/* drain any accumulated messages */
#if 0 /* XXX - not compatible with privsep */
				packet_start(SSH2_MSG_USERAUTH_BANNER);
				packet_put_cstring(**prompts);
				packet_put_cstring("");
				packet_send();
				packet_write_wait();
#endif
				xfree(**prompts);
				**prompts = NULL;
			}
			if (type == PAM_SUCCESS) {
				*num = 0;
				**echo_on = 0;
				ctxt->pam_done = 1;
				xfree(msg);
				return (0);
			}
			error("PAM: %s", msg);
		default:
			*num = 0;
			**echo_on = 0;
			xfree(msg);
			ctxt->pam_done = -1;
			return (-1);
		}
	}
	return (-1);
}