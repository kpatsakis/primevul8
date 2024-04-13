ldns_str2rdf_certificate_usage(ldns_rdf **rd, const char *str)
{
	return ldns_str2rdf_mnemonic4int8(
			ldns_tlsa_certificate_usages, rd, str);
}