monitor_reinit(struct monitor *mon)
{
	int pair[2];

	monitor_socketpair(pair);

	mon->m_recvfd = pair[0];
	mon->m_sendfd = pair[1];
}