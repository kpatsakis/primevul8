mbfl_buffer_converter_feed2(mbfl_buffer_converter *convd, mbfl_string *string, int *loc)
{
	int n;
	unsigned char *p;
	mbfl_convert_filter *filter;
	int (*filter_function)(int c, mbfl_convert_filter *filter);

	if (convd == NULL || string == NULL) {
		return -1;
	}
	mbfl_memory_device_realloc(&convd->device, convd->device.pos + string->len, string->len/4);
	/* feed data */
	n = string->len;
	p = string->val;

	filter = convd->filter1;
	if (filter != NULL) {
		filter_function = filter->filter_function;
		while (n > 0) {
			if ((*filter_function)(*p++, filter) < 0) {
				if (loc) {
					*loc = p - string->val;
				}
				return -1;
			}
			n--;
		}
	}
	if (loc) {
		*loc = p - string->val;
	}
	return 0;
}