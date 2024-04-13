mbfl_html_numeric_entity(
    mbfl_string *string,
    mbfl_string *result,
    int *convmap,
    int mapsize,
    int type)
{
	struct collector_htmlnumericentity_data pc;
	mbfl_memory_device device;
	mbfl_convert_filter *encoder;
	int n;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;
	mbfl_memory_device_init(&device, string->len, 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &device);
	/* wchar filter */
	if (type == 0) { /* decimal output */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_encode_htmlnumericentity, 0, &pc);
	} else if (type == 2) { /* hex output */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_encode_hex_htmlnumericentity, 0, &pc);
	} else { /* type == 1: decimal/hex input */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_decode_htmlnumericentity,
			(int (*)(void*))mbfl_filt_decode_htmlnumericentity_flush, &pc);
	}
	if (pc.decoder == NULL || encoder == NULL) {
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(pc.decoder);
		return NULL;
	}
	pc.status = 0;
	pc.cache = 0;
	pc.digit = 0;
	pc.convmap = convmap;
	pc.mapsize = mapsize;

	/* feed data */
	p = string->val;
	n = string->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(encoder);
	mbfl_convert_filter_flush(pc.decoder);
	result = mbfl_memory_device_result(&device, result);
	mbfl_convert_filter_delete(encoder);
	mbfl_convert_filter_delete(pc.decoder);

	return result;
}