sshpam_cleanup(void *arg)
{
	(void)arg;
	debug("PAM: cleanup");
	pam_set_item(sshpam_handle, PAM_CONV, (const void *)&null_conv);
	if (sshpam_cred_established) {
		pam_setcred(sshpam_handle, PAM_DELETE_CRED);
		sshpam_cred_established = 0;
	}
	if (sshpam_session_open) {
		pam_close_session(sshpam_handle, PAM_SILENT);
		sshpam_session_open = 0;
	}
	sshpam_authenticated = sshpam_new_authtok_reqd = 0;
	pam_end(sshpam_handle, sshpam_err);
	sshpam_handle = NULL;
}