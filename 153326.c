dns_message_setclass(dns_message_t *msg, dns_rdataclass_t rdclass) {

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTPARSE);
	REQUIRE(msg->state == DNS_SECTION_ANY);
	REQUIRE(msg->rdclass_set == 0);

	msg->rdclass = rdclass;
	msg->rdclass_set = 1;
}