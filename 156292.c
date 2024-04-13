do_authentication(void)
{
	Authctxt *authctxt;
	u_int ulen;
	char *user, *style = NULL;

	/* Get the name of the user that we wish to log in as. */
	packet_read_expect(SSH_CMSG_USER);

	/* Get the user name. */
	user = packet_get_string(&ulen);
	packet_check_eom();

	if ((style = strchr(user, ':')) != NULL)
		*style++ = '\0';

#ifdef KRB5
	/* XXX - SSH.com Kerberos v5 braindeath. */
	if ((datafellows & SSH_BUG_K5USER) &&
	    options.kerberos_authentication) {
		char *p;
		if ((p = strchr(user, '@')) != NULL)
			*p = '\0';
	}
#endif

	authctxt = authctxt_new();
	authctxt->user = user;
	authctxt->style = style;

	/* Verify that the user is a valid user. */
	if ((authctxt->pw = PRIVSEP(getpwnamallow(user))) != NULL)
		authctxt->valid = 1;
	else
		debug("do_authentication: illegal user %s", user);

	setproctitle("%s%s", authctxt->pw ? user : "unknown",
	    use_privsep ? " [net]" : "");

#ifdef USE_PAM
	PRIVSEP(start_pam(authctxt->pw == NULL ? "NOUSER" : user));
#endif

	/*
	 * If we are not running as root, the user must have the same uid as
	 * the server. (Unless you are running Windows)
	 */
#ifndef HAVE_CYGWIN
	if (!use_privsep && getuid() != 0 && authctxt->pw &&
	    authctxt->pw->pw_uid != getuid())
		packet_disconnect("Cannot change user when server not running as root.");
#endif

	/*
	 * Loop until the user has been authenticated or the connection is
	 * closed, do_authloop() returns only if authentication is successful
	 */
	do_authloop(authctxt);

	/* The user has been authenticated and accepted. */
	packet_start(SSH_SMSG_SUCCESS);
	packet_send();
	packet_write_wait();

	return (authctxt);
}