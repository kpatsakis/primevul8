dns_message_removename(dns_message_t *msg, dns_name_t *name,
		       dns_section_t section)
{
	REQUIRE(msg != NULL);
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);
	REQUIRE(name != NULL);
	REQUIRE(VALID_NAMED_SECTION(section));

	ISC_LIST_UNLINK(msg->sections[section], name, link);
}