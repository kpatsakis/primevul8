mm_start_pam(char *user)
{
	Buffer m;

	debug3("%s entering", __func__);

	buffer_init(&m);
	buffer_put_cstring(&m, user);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_START, &m);

	buffer_free(&m);
}