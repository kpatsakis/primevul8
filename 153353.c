newrdata(dns_message_t *msg) {
	dns_msgblock_t *msgblock;
	dns_rdata_t *rdata;

	rdata = ISC_LIST_HEAD(msg->freerdata);
	if (rdata != NULL) {
		ISC_LIST_UNLINK(msg->freerdata, rdata, link);
		return (rdata);
	}

	msgblock = ISC_LIST_TAIL(msg->rdatas);
	rdata = msgblock_get(msgblock, dns_rdata_t);
	if (rdata == NULL) {
		msgblock = msgblock_allocate(msg->mctx, sizeof(dns_rdata_t),
					     RDATA_COUNT);
		if (msgblock == NULL)
			return (NULL);

		ISC_LIST_APPEND(msg->rdatas, msgblock, link);

		rdata = msgblock_get(msgblock, dns_rdata_t);
	}

	dns_rdata_init(rdata);
	return (rdata);
}