ldns_str2rdf_matching_type(ldns_rdf **rd, const char *str)
{
	return ldns_str2rdf_mnemonic4int8(ldns_tlsa_matching_types, rd, str);
}