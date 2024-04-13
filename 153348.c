dns_message_currentname(dns_message_t *msg, dns_section_t section,
			dns_name_t **name)
{
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(VALID_NAMED_SECTION(section));
	REQUIRE(name != NULL && *name == NULL);
	REQUIRE(msg->cursors[section] != NULL);

	*name = msg->cursors[section];
}