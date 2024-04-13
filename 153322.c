getname(dns_name_t *name, isc_buffer_t *source, dns_message_t *msg,
	dns_decompress_t *dctx)
{
	isc_buffer_t *scratch;
	isc_result_t result;
	unsigned int tries;

	scratch = currentbuffer(msg);

	/*
	 * First try:  use current buffer.
	 * Second try:  allocate a new buffer and use that.
	 */
	tries = 0;
	while (tries < 2) {
		result = dns_name_fromwire(name, source, dctx, 0,
					   scratch);

		if (result == ISC_R_NOSPACE) {
			tries++;

			result = newbuffer(msg, SCRATCHPAD_SIZE);
			if (result != ISC_R_SUCCESS)
				return (result);

			scratch = currentbuffer(msg);
			dns_name_reset(name);
		} else {
			return (result);
		}
	}

	INSIST(0);
	ISC_UNREACHABLE();
}