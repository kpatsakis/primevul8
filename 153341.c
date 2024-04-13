newrdatalist(dns_message_t *msg) {
	dns_msgblock_t *msgblock;
	dns_rdatalist_t *rdatalist;

	rdatalist = ISC_LIST_HEAD(msg->freerdatalist);
	if (rdatalist != NULL) {
		ISC_LIST_UNLINK(msg->freerdatalist, rdatalist, link);
		goto out;
	}

	msgblock = ISC_LIST_TAIL(msg->rdatalists);
	rdatalist = msgblock_get(msgblock, dns_rdatalist_t);
	if (rdatalist == NULL) {
		msgblock = msgblock_allocate(msg->mctx,
					     sizeof(dns_rdatalist_t),
					     RDATALIST_COUNT);
		if (msgblock == NULL)
			return (NULL);

		ISC_LIST_APPEND(msg->rdatalists, msgblock, link);

		rdatalist = msgblock_get(msgblock, dns_rdatalist_t);
	}
 out:
	if (rdatalist != NULL)
		dns_rdatalist_init(rdatalist);

	return (rdatalist);
}