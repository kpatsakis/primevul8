msginittsig(dns_message_t *m) {
	m->tsigstatus = dns_rcode_noerror;
	m->querytsigstatus = dns_rcode_noerror;
	m->tsigkey = NULL;
	m->tsigctx = NULL;
	m->sigstart = -1;
	m->sig0key = NULL;
	m->sig0status = dns_rcode_noerror;
	m->timeadjust = 0;
}