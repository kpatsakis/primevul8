ldns_str2rdf_mnemonic4int8(ldns_lookup_table *lt,
		ldns_rdf **rd, const char *str)
{
	if ((lt = ldns_lookup_by_name(lt, str))) {
		/* it was given as a integer */
		*rd = ldns_native2rdf_int8(LDNS_RDF_TYPE_INT8, (uint8_t) lt->id);
		if (!*rd)
			return LDNS_STATUS_ERR;
		else
			return LDNS_STATUS_OK;
	}
	return ldns_str2rdf_int8(rd, str);
}