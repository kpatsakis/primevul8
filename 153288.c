dns_message_rechecksig(dns_message_t *msg, dns_view_t *view) {
	dns_message_resetsig(msg);
	return (dns_message_checksig(msg, view));
}