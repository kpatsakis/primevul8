ipmi_print_channel_cipher_suites(struct ipmi_intf *intf,
                                 const char *payload_type,
                                 uint8_t channel)
{
	int rc;
	size_t i = 0;
	struct cipher_suite_info suites[MAX_CIPHER_SUITE_COUNT];
	size_t nr_suites = sizeof(*suites);
	const char *header_str =
"ID   IANA    Auth Alg        Integrity Alg   Confidentiality Alg";

	rc = ipmi_get_channel_cipher_suites(intf, payload_type, channel,
	                                    suites, &nr_suites);

	if (rc < 0)
		return rc;

	if (!csv_output) {
		printf("%s\n", header_str);
	}
	for (i = 0; i < nr_suites; i++) {
		/* We have everything we need to spit out a cipher suite record */
		printf(csv_output ? "%d,%s,%s,%s,%s\n"
		                  : "%-4d %-7s %-15s %-15s %-15s\n",
		       suites[i].cipher_suite_id,
		       iana_string(suites[i].iana),
		       val2str(suites[i].auth_alg, ipmi_auth_algorithms),
		       val2str(suites[i].integrity_alg, ipmi_integrity_algorithms),
		       val2str(suites[i].crypt_alg, ipmi_encryption_algorithms));
	}
	return 0;
}