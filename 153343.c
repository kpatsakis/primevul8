releaserdatalist(dns_message_t *msg, dns_rdatalist_t *rdatalist) {
	ISC_LIST_PREPEND(msg->freerdatalist, rdatalist, link);
}