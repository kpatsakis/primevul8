mm_answer_pam_free_ctx(int socket, Buffer *m)
{

	debug3("%s", __func__);
	(sshpam_device.free_ctx)(sshpam_ctxt);
	buffer_clear(m);
	mm_request_send(socket, MONITOR_ANS_PAM_FREE_CTX, m);
	return (sshpam_authok == sshpam_ctxt);
}