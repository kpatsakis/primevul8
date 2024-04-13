msginitprivate(dns_message_t *m) {
	unsigned int i;

	for (i = 0; i < DNS_SECTION_MAX; i++) {
		m->cursors[i] = NULL;
		m->counts[i] = 0;
	}
	m->opt = NULL;
	m->sig0 = NULL;
	m->sig0name = NULL;
	m->tsig = NULL;
	m->tsigname = NULL;
	m->state = DNS_SECTION_ANY;  /* indicate nothing parsed or rendered */
	m->opt_reserved = 0;
	m->sig_reserved = 0;
	m->reserved = 0;
	m->buffer = NULL;
}