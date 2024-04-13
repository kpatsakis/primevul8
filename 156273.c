monitor_init(void)
{
	struct monitor *mon;
	int pair[2];

	mon = xmalloc(sizeof(*mon));

	monitor_socketpair(pair);

	mon->m_recvfd = pair[0];
	mon->m_sendfd = pair[1];

	/* Used to share zlib space across processes */
	if (options.compression) {
		mon->m_zback = mm_create(NULL, MM_MEMSIZE);
		mon->m_zlib = mm_create(mon->m_zback, 20 * MM_MEMSIZE);

		/* Compression needs to share state across borders */
		mm_init_compression(mon->m_zlib);
	}

	return mon;
}