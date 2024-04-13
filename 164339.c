mbfl_convert_encoding(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding toenc)
{
	int n;
	unsigned char *p;
	const mbfl_encoding *encoding;
	mbfl_memory_device device;
	mbfl_convert_filter *filter1;
	mbfl_convert_filter *filter2;

	/* initialize */
	encoding = mbfl_no2encoding(toenc);
	if (encoding == NULL || string == NULL || result == NULL) {
		return NULL;
	}

	filter1 = NULL;
	filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(string->no_encoding, toenc) != NULL) {
		filter1 = mbfl_convert_filter_new(string->no_encoding, toenc, mbfl_memory_device_output, 0, &device);
	} else {
		filter2 = mbfl_convert_filter_new(mbfl_no_encoding_wchar, toenc, mbfl_memory_device_output, 0, &device);
		if (filter2 != NULL) {
			filter1 = mbfl_convert_filter_new(string->no_encoding, mbfl_no_encoding_wchar, (int (*)(int, void*))filter2->filter_function, NULL, filter2);
			if (filter1 == NULL) {
				mbfl_convert_filter_delete(filter2);
			}
		}
	}
	if (filter1 == NULL) {
		return NULL;
	}

	if (filter2 != NULL) {
		filter2->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
		filter2->illegal_substchar = 0x3f;		/* '?' */
	}

	mbfl_memory_device_init(&device, string->len, (string->len >> 2) + 8);

	/* feed data */
	n = string->len;
	p = string->val;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter1->filter_function)(*p++, filter1) < 0) {
				break;
			}
			n--;
		}
	}

	mbfl_convert_filter_flush(filter1);
	mbfl_convert_filter_delete(filter1);
	if (filter2 != NULL) {
		mbfl_convert_filter_flush(filter2);
		mbfl_convert_filter_delete(filter2);
	}

	return mbfl_memory_device_result(&device, result);
}