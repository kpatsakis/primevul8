dns_message_nextname(dns_message_t *msg, dns_section_t section) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(VALID_NAMED_SECTION(section));
	REQUIRE(msg->cursors[section] != NULL);

	msg->cursors[section] = ISC_LIST_NEXT(msg->cursors[section], link);

	if (msg->cursors[section] == NULL)
		return (ISC_R_NOMORE);

	return (ISC_R_SUCCESS);
}