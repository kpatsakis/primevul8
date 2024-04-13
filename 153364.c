dns_message_firstname(dns_message_t *msg, dns_section_t section) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(VALID_NAMED_SECTION(section));

	msg->cursors[section] = ISC_LIST_HEAD(msg->sections[section]);

	if (msg->cursors[section] == NULL)
		return (ISC_R_NOMORE);

	return (ISC_R_SUCCESS);
}