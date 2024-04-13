mbfl_buffer_converter_strncat(mbfl_buffer_converter *convd, const unsigned char *p, int n)
{
	mbfl_convert_filter *filter;
	int (*filter_function)(int c, mbfl_convert_filter *filter);

	if (convd != NULL && p != NULL) {
		filter = convd->filter1;
		if (filter != NULL) {
			filter_function = filter->filter_function;
			while (n > 0) {
				if ((*filter_function)(*p++, filter) < 0) {
					break;
				}
				n--;
			}
		}
	}

	return n;
}