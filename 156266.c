do_pam_session(const char *user, const char *tty)
{
	sshpam_err = pam_set_item(sshpam_handle, PAM_CONV, 
	    (const void *)&null_conv);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: failed to set PAM_CONV: %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	debug("PAM: setting PAM_TTY to \"%s\"", tty);
	sshpam_err = pam_set_item(sshpam_handle, PAM_TTY, tty);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: failed to set PAM_TTY: %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	sshpam_err = pam_open_session(sshpam_handle, 0);
	if (sshpam_err != PAM_SUCCESS)
		fatal("PAM: pam_open_session(): %s",
		    pam_strerror(sshpam_handle, sshpam_err));
	sshpam_session_open = 1;
}