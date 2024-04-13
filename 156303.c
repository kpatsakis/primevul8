mm_answer_pwnamallow(int socket, Buffer *m)
{
	char *login;
	struct passwd *pwent;
	int allowed = 0;

	debug3("%s", __func__);

	if (authctxt->attempt++ != 0)
		fatal("%s: multiple attempts for getpwnam", __func__);

	login = buffer_get_string(m, NULL);

	pwent = getpwnamallow(login);

	authctxt->user = xstrdup(login);
	setproctitle("%s [priv]", pwent ? login : "unknown");
	xfree(login);

	buffer_clear(m);

	if (pwent == NULL) {
		buffer_put_char(m, 0);
		goto out;
	}

	allowed = 1;
	authctxt->pw = pwent;
	authctxt->valid = 1;

	buffer_put_char(m, 1);
	buffer_put_string(m, pwent, sizeof(struct passwd));
	buffer_put_cstring(m, pwent->pw_name);
	buffer_put_cstring(m, "*");
	buffer_put_cstring(m, pwent->pw_gecos);
#ifdef HAVE_PW_CLASS_IN_PASSWD
	buffer_put_cstring(m, pwent->pw_class);
#endif
	buffer_put_cstring(m, pwent->pw_dir);
	buffer_put_cstring(m, pwent->pw_shell);

 out:
	debug3("%s: sending MONITOR_ANS_PWNAM: %d", __func__, allowed);
	mm_request_send(socket, MONITOR_ANS_PWNAM, m);

	/* For SSHv1 allow authentication now */
	if (!compat20)
		monitor_permit_authentications(1);
	else {
		/* Allow service/style information on the auth context */
		monitor_permit(mon_dispatch, MONITOR_REQ_AUTHSERV, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_AUTH2_READ_BANNER, 1);
	}

#ifdef USE_PAM
	monitor_permit(mon_dispatch, MONITOR_REQ_PAM_START, 1);
#endif

	return (0);
}