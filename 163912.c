ldns_str2rdf_eui64(ldns_rdf **rd, const char *str)
{
	unsigned int a, b, c, d, e, f, g, h;
	uint8_t bytes[8];
	int l;

	if (sscanf(str, "%2x-%2x-%2x-%2x-%2x-%2x-%2x-%2x%n",
			&a, &b, &c, &d, &e, &f, &g, &h, &l) != 8 ||
			l != (int)strlen(str)) {
		return LDNS_STATUS_INVALID_EUI64;
	} else {
		bytes[0] = a;
		bytes[1] = b;
		bytes[2] = c;
		bytes[3] = d;
		bytes[4] = e;
		bytes[5] = f;
		bytes[6] = g;
		bytes[7] = h;
		*rd = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_EUI64, 8, &bytes);
	}
	return *rd ? LDNS_STATUS_OK : LDNS_STATUS_MEM_ERR;
}