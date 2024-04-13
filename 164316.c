int mbfl_buffer_illegalchars(mbfl_buffer_converter *convd)
{
	int num_illegalchars = 0;

	if (convd == NULL) {
		return 0;
	}

	if (convd->filter1 != NULL) {
		num_illegalchars += convd->filter1->num_illegalchar;
	}

	if (convd->filter2 != NULL) {
		num_illegalchars += convd->filter2->num_illegalchar;
	}

	return (num_illegalchars);
}