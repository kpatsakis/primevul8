currentbuffer(dns_message_t *msg) {
	isc_buffer_t *dynbuf;

	dynbuf = ISC_LIST_TAIL(msg->scratchpad);
	INSIST(dynbuf != NULL);

	return (dynbuf);
}