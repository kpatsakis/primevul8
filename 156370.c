mm_pty_allocate(int *ptyfd, int *ttyfd, char *namebuf, int namebuflen)
{
	Buffer m;
	char *p;
	int success = 0;

	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PTY, &m);

	debug3("%s: waiting for MONITOR_ANS_PTY", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PTY, &m);

	success = buffer_get_int(&m);
	if (success == 0) {
		debug3("%s: pty alloc failed", __func__);
		buffer_free(&m);
		return (0);
	}
	p = buffer_get_string(&m, NULL);
	buffer_free(&m);

	strlcpy(namebuf, p, namebuflen); /* Possible truncation */
	xfree(p);

	*ptyfd = mm_receive_fd(pmonitor->m_recvfd);
	*ttyfd = mm_receive_fd(pmonitor->m_recvfd);

	/* Success */
	return (1);
}