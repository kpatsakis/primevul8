dns_message_movename(dns_message_t *msg, dns_name_t *name,
		     dns_section_t fromsection,
		     dns_section_t tosection)
{
	REQUIRE(msg != NULL);
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);
	REQUIRE(name != NULL);
	REQUIRE(VALID_NAMED_SECTION(fromsection));
	REQUIRE(VALID_NAMED_SECTION(tosection));

	/*
	 * Unlink the name from the old section
	 */
	ISC_LIST_UNLINK(msg->sections[fromsection], name, link);
	ISC_LIST_APPEND(msg->sections[tosection], name, link);
}