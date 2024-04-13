render_ecs(isc_buffer_t *ecsbuf, isc_buffer_t *target) {
	int i;
	char addr[16], addr_text[64];
	uint16_t family;
	uint8_t addrlen, addrbytes, scopelen;
	isc_result_t result;

	/*
	 * Note: This routine needs to handle malformed ECS options.
	 */

	if (isc_buffer_remaininglength(ecsbuf) < 4)
		return (DNS_R_OPTERR);
	family = isc_buffer_getuint16(ecsbuf);
	addrlen = isc_buffer_getuint8(ecsbuf);
	scopelen = isc_buffer_getuint8(ecsbuf);

	addrbytes = (addrlen + 7) / 8;
	if (isc_buffer_remaininglength(ecsbuf) < addrbytes)
		return (DNS_R_OPTERR);

	if (addrbytes > sizeof(addr))
		return (DNS_R_OPTERR);

	memset(addr, 0, sizeof(addr));
	for (i = 0; i < addrbytes; i ++)
		addr[i] = isc_buffer_getuint8(ecsbuf);

	switch (family) {
	case 0:
		if (addrlen != 0U || scopelen != 0U)
			return (DNS_R_OPTERR);
		strlcpy(addr_text, "0", sizeof(addr_text));
		break;
	case 1:
		if (addrlen > 32 || scopelen > 32)
			return (DNS_R_OPTERR);
		inet_ntop(AF_INET, addr, addr_text, sizeof(addr_text));
		break;
	case 2:
		if (addrlen > 128 || scopelen > 128)
			return (DNS_R_OPTERR);
		inet_ntop(AF_INET6, addr, addr_text, sizeof(addr_text));
		break;
	default:
		return (DNS_R_OPTERR);
	}

	ADD_STRING(target, ": ");
	ADD_STRING(target, addr_text);
	snprintf(addr_text, sizeof(addr_text), "/%d/%d", addrlen, scopelen);
	ADD_STRING(target, addr_text);

	result = ISC_R_SUCCESS;

 cleanup:
	return (result);
}