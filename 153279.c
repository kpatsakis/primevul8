dns_message_renderheader(dns_message_t *msg, isc_buffer_t *target) {
	uint16_t tmp;
	isc_region_t r;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(target != NULL);

	isc_buffer_availableregion(target, &r);
	REQUIRE(r.length >= DNS_MESSAGE_HEADERLEN);

	isc_buffer_putuint16(target, msg->id);

	tmp = ((msg->opcode << DNS_MESSAGE_OPCODE_SHIFT)
	       & DNS_MESSAGE_OPCODE_MASK);
	tmp |= (msg->rcode & DNS_MESSAGE_RCODE_MASK);
	tmp |= (msg->flags & DNS_MESSAGE_FLAG_MASK);

	INSIST(msg->counts[DNS_SECTION_QUESTION]  < 65536 &&
	       msg->counts[DNS_SECTION_ANSWER]    < 65536 &&
	       msg->counts[DNS_SECTION_AUTHORITY] < 65536 &&
	       msg->counts[DNS_SECTION_ADDITIONAL] < 65536);

	isc_buffer_putuint16(target, tmp);
	isc_buffer_putuint16(target,
			    (uint16_t)msg->counts[DNS_SECTION_QUESTION]);
	isc_buffer_putuint16(target,
			    (uint16_t)msg->counts[DNS_SECTION_ANSWER]);
	isc_buffer_putuint16(target,
			    (uint16_t)msg->counts[DNS_SECTION_AUTHORITY]);
	isc_buffer_putuint16(target,
			    (uint16_t)msg->counts[DNS_SECTION_ADDITIONAL]);
}