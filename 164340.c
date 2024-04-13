mbfl_substr_count(
    mbfl_string *haystack,
    mbfl_string *needle
   )
{
	int n, result = 0;
	unsigned char *p;
	mbfl_convert_filter *filter;
	struct collector_strpos_data pc;

	if (haystack == NULL || needle == NULL) {
		return -8;
	}
	/* needle is converted into wchar */
	mbfl_wchar_device_init(&pc.needle);
	filter = mbfl_convert_filter_new(
	  needle->no_encoding,
	  mbfl_no_encoding_wchar,
	  mbfl_wchar_device_output, 0, &pc.needle);
	if (filter == NULL) {
		return -4;
	}
	p = needle->val;
	n = needle->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				break;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(filter);
	mbfl_convert_filter_delete(filter);
	pc.needle_len = pc.needle.pos;
	if (pc.needle.buffer == NULL) {
		return -4;
	}
	if (pc.needle_len <= 0) {
		mbfl_wchar_device_clear(&pc.needle);
		return -2;
	}
	/* initialize filter and collector data */
	filter = mbfl_convert_filter_new(
	  haystack->no_encoding,
	  mbfl_no_encoding_wchar,
	  collector_strpos, 0, &pc);
	if (filter == NULL) {
		mbfl_wchar_device_clear(&pc.needle);
		return -4;
	}
	pc.start = 0;
	pc.output = 0;
	pc.needle_pos = 0;
	pc.found_pos = 0;
	pc.matched_pos = -1;

	/* feed data */
	p = haystack->val;
	n = haystack->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				pc.matched_pos = -4;
				break;
			}
			if (pc.matched_pos >= 0) {
				++result;
				pc.matched_pos = -1;
				pc.needle_pos = 0;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(filter);
	mbfl_convert_filter_delete(filter);
	mbfl_wchar_device_clear(&pc.needle);

	return result;
}