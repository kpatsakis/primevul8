mbfl_strwidth(mbfl_string *string)
{
	int len, n;
	unsigned char *p;
	mbfl_convert_filter *filter;

	len = 0;
	if (string->len > 0 && string->val != NULL) {
		/* wchar filter */
		filter = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    filter_count_width, 0, &len);
		if (filter == NULL) {
			mbfl_convert_filter_delete(filter);
			return -1;
		}

		/* feed data */
		p = string->val;
		n = string->len;
		while (n > 0) {
			(*filter->filter_function)(*p++, filter);
			n--;
		}

		mbfl_convert_filter_flush(filter);
		mbfl_convert_filter_delete(filter);
	}

	return len;
}