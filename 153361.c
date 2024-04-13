msginitheader(dns_message_t *m) {
	m->id = 0;
	m->flags = 0;
	m->rcode = 0;
	m->opcode = 0;
	m->rdclass = 0;
}