dns_opcode_totext(dns_opcode_t opcode, isc_buffer_t *target) {

	REQUIRE(opcode < 16);

	if (isc_buffer_availablelength(target) < strlen(opcodetext[opcode]))
		return (ISC_R_NOSPACE);
	isc_buffer_putstr(target, opcodetext[opcode]);
	return (ISC_R_SUCCESS);
}