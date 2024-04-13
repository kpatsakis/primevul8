ldns_str2rdf_alg(ldns_rdf **rd, const char *str)
{
	return ldns_str2rdf_mnemonic4int8(ldns_algorithms, rd, str);
}