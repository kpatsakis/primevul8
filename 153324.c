getrdata(isc_buffer_t *source, dns_message_t *msg, dns_decompress_t *dctx,
	 dns_rdataclass_t rdclass, dns_rdatatype_t rdtype,
	 unsigned int rdatalen, dns_rdata_t *rdata)
{
	isc_buffer_t *scratch;
	isc_result_t result;
	unsigned int tries;
	unsigned int trysize;

	scratch = currentbuffer(msg);

	isc_buffer_setactive(source, rdatalen);

	/*
	 * First try:  use current buffer.
	 * Second try:  allocate a new buffer of size
	 *     max(SCRATCHPAD_SIZE, 2 * compressed_rdatalen)
	 *     (the data will fit if it was not more than 50% compressed)
	 * Subsequent tries: double buffer size on each try.
	 */
	tries = 0;
	trysize = 0;
	/* XXX possibly change this to a while (tries < 2) loop */
	for (;;) {
		result = dns_rdata_fromwire(rdata, rdclass, rdtype,
					    source, dctx, 0,
					    scratch);

		if (result == ISC_R_NOSPACE) {
			if (tries == 0) {
				trysize = 2 * rdatalen;
				if (trysize < SCRATCHPAD_SIZE)
					trysize = SCRATCHPAD_SIZE;
			} else {
				INSIST(trysize != 0);
				if (trysize >= 65535)
					return (ISC_R_NOSPACE);
					/* XXX DNS_R_RRTOOLONG? */
				trysize *= 2;
			}
			tries++;
			result = newbuffer(msg, trysize);
			if (result != ISC_R_SUCCESS)
				return (result);

			scratch = currentbuffer(msg);
		} else {
			return (result);
		}
	}
}