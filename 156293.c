mm_terminate(void)
{
	Buffer m;

	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_TERM, &m);
	buffer_free(&m);
}