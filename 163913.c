ldns_str2rdf_selector(ldns_rdf **rd, const char *str)
{
	return ldns_str2rdf_mnemonic4int8(ldns_tlsa_selectors, rd, str);
}