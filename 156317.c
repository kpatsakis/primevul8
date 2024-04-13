sshpam_thread(void *ctxtp)
{
	struct pam_ctxt *ctxt = ctxtp;
	Buffer buffer;
	struct pam_conv sshpam_conv = { sshpam_thread_conv, ctxt };
#ifndef USE_POSIX_THREADS
	const char *pam_user;

	pam_get_item(sshpam_handle, PAM_USER, (const void **)&pam_user);
	setproctitle("%s [pam]", pam_user);
#endif

	buffer_init(&buffer);
	sshpam_err = pam_set_item(sshpam_handle, PAM_CONV,
	    (const void *)&sshpam_conv);
	if (sshpam_err != PAM_SUCCESS)
		goto auth_fail;
	sshpam_err = pam_authenticate(sshpam_handle, 0);
	if (sshpam_err != PAM_SUCCESS)
		goto auth_fail;
	sshpam_err = pam_acct_mgmt(sshpam_handle, 0);
	if (sshpam_err != PAM_SUCCESS && sshpam_err != PAM_NEW_AUTHTOK_REQD)
		goto auth_fail;
	buffer_put_cstring(&buffer, "OK");
	ssh_msg_send(ctxt->pam_csock, sshpam_err, &buffer);
	buffer_free(&buffer);
	pthread_exit(NULL);

 auth_fail:
	buffer_put_cstring(&buffer,
	    pam_strerror(sshpam_handle, sshpam_err));
	ssh_msg_send(ctxt->pam_csock, PAM_AUTH_ERR, &buffer);
	buffer_free(&buffer);
	pthread_exit(NULL);
	
	return (NULL); /* Avoid warning for non-pthread case */
}