mbfl_encoding_detector_delete(mbfl_encoding_detector *identd)
{
	int i;

	if (identd != NULL) {
		if (identd->filter_list != NULL) {
			i = identd->filter_list_size;
			while (i > 0) {
				i--;
				mbfl_identify_filter_delete(identd->filter_list[i]);
			}
			mbfl_free((void *)identd->filter_list);
		}
		mbfl_free((void *)identd);
	}
}