mm_getpwnamallow(const char *login)
{
	Buffer m;
	struct passwd *pw;
	u_int pwlen;

	debug3("%s entering", __func__);

	buffer_init(&m);
	buffer_put_cstring(&m, login);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PWNAM, &m);

	debug3("%s: waiting for MONITOR_ANS_PWNAM", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PWNAM, &m);

	if (buffer_get_char(&m) == 0) {
		buffer_free(&m);
		return (NULL);
	}
	pw = buffer_get_string(&m, &pwlen);
	if (pwlen != sizeof(struct passwd))
		fatal("%s: struct passwd size mismatch", __func__);
	pw->pw_name = buffer_get_string(&m, NULL);
	pw->pw_passwd = buffer_get_string(&m, NULL);
	pw->pw_gecos = buffer_get_string(&m, NULL);
#ifdef HAVE_PW_CLASS_IN_PASSWD
	pw->pw_class = buffer_get_string(&m, NULL);
#endif
	pw->pw_dir = buffer_get_string(&m, NULL);
	pw->pw_shell = buffer_get_string(&m, NULL);
	buffer_free(&m);

	return (pw);
}