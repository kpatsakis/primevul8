mbfl_buffer_converter_illegal_substchar(mbfl_buffer_converter *convd, int substchar)
{
	if (convd != NULL) {
		if (convd->filter2 != NULL) {
			convd->filter2->illegal_substchar = substchar;
		} else if (convd->filter1 != NULL) {
			convd->filter1->illegal_substchar = substchar;
		} else {
			return 0;
		}
	}

	return 1;
}