mbfl_buffer_converter_illegal_mode(mbfl_buffer_converter *convd, int mode)
{
	if (convd != NULL) {
		if (convd->filter2 != NULL) {
			convd->filter2->illegal_mode = mode;
		} else if (convd->filter1 != NULL) {
			convd->filter1->illegal_mode = mode;
		} else {
			return 0;
		}
	}

	return 1;
}