do_pam_setcred(int init)
{
	sshpam_err = pam_set_item(sshpam_handle, PAM_CONV,
	    (const void *)&null_conv);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: failed to set PAM_CONV: %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	if (init) {
		debug("PAM: establishing credentials");
		sshpam_err = pam_setcred(sshpam_handle, PAM_ESTABLISH_CRED);
	} else {
		debug("PAM: reinitializing credentials");
		sshpam_err = pam_setcred(sshpam_handle, PAM_REINITIALIZE_CRED);
	}
	if (sshpam_err == PAM_SUCCESS) {
		sshpam_cred_established = 1;
		return;
	}
	if (sshpam_authenticated)
		fatal("PAM: pam_setcred(): %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	else
		debug("PAM: pam_setcred(): %s",
		    pam_strerror(sshpam_handle, sshpam_err));
}