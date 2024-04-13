ldns_str2rdf_str(ldns_rdf **rd, const char *str)
{
	uint8_t *data, *dp, ch = 0;
	size_t length;

	/* Worst case space requirement. We'll realloc to actual size later. */
	dp = data = LDNS_XMALLOC(uint8_t, strlen(str) > 255 ? 256 : (strlen(str) + 1));
	if (! data) {
		return LDNS_STATUS_MEM_ERR;
	}

	/* Fill data (up to 255 characters) */
	while (parse_char(&ch, &str)) {
		if (dp - data >= 255) {
			LDNS_FREE(data);
			return LDNS_STATUS_INVALID_STR;
		}
		*++dp = ch;
	}
	if (! str) {
		return LDNS_STATUS_SYNTAX_BAD_ESCAPE;
	}
	length = (size_t)(dp - data);
	/* Fix last length byte */
	data[0] = (uint8_t)length;

	/* Lose the overmeasure */
	data = LDNS_XREALLOC(dp = data, uint8_t, length + 1);
	if (! data) {
		LDNS_FREE(dp);
		return LDNS_STATUS_MEM_ERR;
	}

	/* Create rdf */
	*rd = ldns_rdf_new(LDNS_RDF_TYPE_STR, length + 1, data);
	if (! *rd) {
		LDNS_FREE(data);
		return LDNS_STATUS_MEM_ERR;
	}
	return LDNS_STATUS_OK;
}