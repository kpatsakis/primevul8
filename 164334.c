mbfl_mime_header_encode(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding encoding,
    const char *linefeed,
    int indent)
{
	int n;
	unsigned char *p;
	struct mime_header_encoder_data *pe;

	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = mbfl_no_encoding_ascii;

	pe = mime_header_encoder_new(string->no_encoding, outcode, encoding);
	if (pe == NULL) {
		return NULL;
	}

	if (linefeed != NULL) {
		n = 0;
		while (*linefeed && n < 8) {
			pe->lwsp[n++] = *linefeed++;
		}
		pe->lwsp[n++] = 0x20;
		pe->lwsp[n] = '\0';
		pe->lwsplen = n;
	}
	if (indent > 0 && indent < 74) {
		pe->firstindent = indent;
	}

	n = string->len;
	p = string->val;
	while (n > 0) {
		(*pe->conv1_filter->filter_function)(*p++, pe->conv1_filter);
		n--;
	}

	result = mime_header_encoder_result(pe, result);
	mime_header_encoder_delete(pe);

	return result;
}