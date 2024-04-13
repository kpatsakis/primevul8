parse_channel_cipher_suite_data(uint8_t *cipher_suite_data, size_t data_len,
                                struct cipher_suite_info* suites,
                                size_t nr_suites)
{
	size_t count = 0;
	size_t offset = 0;

	/* Default everything to zeroes */
	memset(suites, 0, sizeof(*suites) * nr_suites);

	while (offset < data_len && count < nr_suites) {
		size_t suite_size;

		/* Set non-zero defaults */
		suites[count].auth_alg = IPMI_AUTH_RAKP_NONE;
		suites[count].integrity_alg = IPMI_INTEGRITY_NONE;
		suites[count].crypt_alg = IPMI_CRYPT_NONE;

		/* Update fields from cipher suite data */
		suite_size = parse_cipher_suite(cipher_suite_data + offset,
		                                data_len - offset,
		                                &suites[count].iana,
		                                &suites[count].auth_alg,
		                                &suites[count].integrity_alg,
		                                &suites[count].crypt_alg,
		                                &suites[count].cipher_suite_id);

		if (!suite_size) {
			lprintf(LOG_INFO,
			        "Failed to parse cipher suite data at offset %d",
			        offset);
			break;
		}

		offset += suite_size;
		count++;
	}
	return count;
}