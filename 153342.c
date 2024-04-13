releaserdata(dns_message_t *msg, dns_rdata_t *rdata) {
	ISC_LIST_PREPEND(msg->freerdata, rdata, link);
}