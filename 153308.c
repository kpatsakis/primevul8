dns_message_renderreset(dns_message_t *msg) {
	unsigned int i;
	dns_name_t *name;
	dns_rdataset_t *rds;

	/*
	 * Reset the message so that it may be rendered again.
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);

	msg->buffer = NULL;

	for (i = 0; i < DNS_SECTION_MAX; i++) {
		msg->cursors[i] = NULL;
		msg->counts[i] = 0;
		for (name = ISC_LIST_HEAD(msg->sections[i]);
		     name != NULL;
		     name = ISC_LIST_NEXT(name, link)) {
			for (rds = ISC_LIST_HEAD(name->list);
			     rds != NULL;
			     rds = ISC_LIST_NEXT(rds, link)) {
				rds->attributes &= ~DNS_RDATASETATTR_RENDERED;
			}
		}
	}
	if (msg->tsigname != NULL)
		dns_message_puttempname(msg, &msg->tsigname);
	if (msg->tsig != NULL) {
		dns_rdataset_disassociate(msg->tsig);
		dns_message_puttemprdataset(msg, &msg->tsig);
	}
	if (msg->sig0 != NULL) {
		dns_rdataset_disassociate(msg->sig0);
		dns_message_puttemprdataset(msg, &msg->sig0);
	}
}