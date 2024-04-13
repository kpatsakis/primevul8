sshpam_init(const char *user)
{
	extern ServerOptions options;
	extern u_int utmp_len;
	const char *pam_rhost, *pam_user;

	if (sshpam_handle != NULL) {
		/* We already have a PAM context; check if the user matches */
		sshpam_err = pam_get_item(sshpam_handle,
		    PAM_USER, (const void **)&pam_user);
		if (sshpam_err == PAM_SUCCESS && strcmp(user, pam_user) == 0)
			return (0);
		fatal_remove_cleanup(sshpam_cleanup, NULL);
		pam_end(sshpam_handle, sshpam_err);
		sshpam_handle = NULL;
	}
	debug("PAM: initializing for \"%s\"", user);
	sshpam_err = pam_start("sshd", user, &null_conv, &sshpam_handle);
	if (sshpam_err != PAM_SUCCESS)
		return (-1);
	pam_rhost = get_remote_name_or_ip(utmp_len,
	    options.verify_reverse_mapping);
	debug("PAM: setting PAM_RHOST to \"%s\"", pam_rhost);
	sshpam_err = pam_set_item(sshpam_handle, PAM_RHOST, pam_rhost);
	if (sshpam_err != PAM_SUCCESS) {
		pam_end(sshpam_handle, sshpam_err);
		sshpam_handle = NULL;
		return (-1);
	}
	fatal_add_cleanup(sshpam_cleanup, NULL);
	return (0);
}