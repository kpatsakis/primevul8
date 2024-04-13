mbfl_buffer_converter_flush(mbfl_buffer_converter *convd)
{
	if (convd == NULL) {
		return -1;
	}

	if (convd->filter1 != NULL) {
		mbfl_convert_filter_flush(convd->filter1);
	}
	if (convd->filter2 != NULL) {
		mbfl_convert_filter_flush(convd->filter2);
	}

	return 0;
}