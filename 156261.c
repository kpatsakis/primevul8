sshpam_respond(void *ctx, u_int num, char **resp)
{
	Buffer buffer;
	struct pam_ctxt *ctxt = ctx;

	debug2("PAM: %s", __func__);
	switch (ctxt->pam_done) {
	case 1:
		sshpam_authenticated = 1;
		return (0);
	case 0:
		break;
	default:
		return (-1);
	}
	if (num != 1) {
		error("PAM: expected one response, got %u", num);
		return (-1);
	}
	buffer_init(&buffer);
	buffer_put_cstring(&buffer, *resp);
	ssh_msg_send(ctxt->pam_psock, PAM_AUTHTOK, &buffer);
	buffer_free(&buffer);
	return (1);
}