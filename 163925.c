ldns_str2rdf_tag(ldns_rdf **rd, const char *str)
{
	uint8_t *data;
	const char* ptr;

	if (strlen(str) > 255) {
		return LDNS_STATUS_INVALID_TAG;
	}
	for (ptr = str; *ptr; ptr++) {
		if (! isalnum((unsigned char)*ptr)) {
			return LDNS_STATUS_INVALID_TAG;
		}
	}
	data = LDNS_XMALLOC(uint8_t, strlen(str) + 1);
        if (!data) {
		return LDNS_STATUS_MEM_ERR;
	}
	data[0] = strlen(str);
	memcpy(data + 1, str, strlen(str));

	*rd = ldns_rdf_new(LDNS_RDF_TYPE_TAG, strlen(str) + 1, data);
	if (!*rd) {
		LDNS_FREE(data);
		return LDNS_STATUS_MEM_ERR;
	}
	return LDNS_STATUS_OK;
}