mbfl_mime_header_decode(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding outcode)
{
	int n;
	unsigned char *p;
	struct mime_header_decoder_data *pd;

	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = outcode;

	pd = mime_header_decoder_new(outcode);
	if (pd == NULL) {
		return NULL;
	}

	/* feed data */
	n = string->len;
	p = string->val;
	while (n > 0) {
		mime_header_decoder_collector(*p++, pd);
		n--;
	}

	result = mime_header_decoder_result(pd, result);
	mime_header_decoder_delete(pd);

	return result;
}