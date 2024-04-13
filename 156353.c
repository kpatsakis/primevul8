do_pam_chauthtok(void)
{
	struct pam_conv pam_conv = { pam_chauthtok_conv, NULL };

	if (use_privsep)
		fatal("PAM: chauthtok not supprted with privsep");
	sshpam_err = pam_set_item(sshpam_handle, PAM_CONV,
	    (const void *)&pam_conv);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: failed to set PAM_CONV: %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	debug("PAM: changing password");
	sshpam_err = pam_chauthtok(sshpam_handle, PAM_CHANGE_EXPIRED_AUTHTOK);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: pam_chauthtok(): %s",
		    pam_strerror(sshpam_handle, sshpam_err));
}