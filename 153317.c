render_llq(isc_buffer_t *optbuf, isc_buffer_t *target) {
	char buf[sizeof("18446744073709551615")]; /* 2^64-1 */
	isc_result_t result = ISC_R_SUCCESS;
	uint32_t u;
	uint64_t q;

	u = isc_buffer_getuint16(optbuf);
	ADD_STRING(target, " Version: ");
	snprintf(buf, sizeof(buf), "%u", u);
	ADD_STRING(target, buf);

	u = isc_buffer_getuint16(optbuf);
	ADD_STRING(target, ", Opcode: ");
	snprintf(buf, sizeof(buf), "%u", u);
	ADD_STRING(target, buf);

	u = isc_buffer_getuint16(optbuf);
	ADD_STRING(target, ", Error: ");
	snprintf(buf, sizeof(buf), "%u", u);
	ADD_STRING(target, buf);

	q = isc_buffer_getuint32(optbuf);
	q <<= 32;
	q |= isc_buffer_getuint32(optbuf);
	ADD_STRING(target, ", Identifier: ");
	snprintf(buf, sizeof(buf), "%" PRIu64, q);
	ADD_STRING(target, buf);

	u = isc_buffer_getuint32(optbuf);
	ADD_STRING(target, ", Lifetime: ");
	snprintf(buf, sizeof(buf), "%u", u);
	ADD_STRING(target, buf);
 cleanup:
	return (result);
}