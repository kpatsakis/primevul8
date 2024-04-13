mbfl_encoding_detector_new2(const mbfl_encoding **elist, int elistsz, int strict)
{
	mbfl_encoding_detector *identd;

	int i, num;
	mbfl_identify_filter *filter;

	if (elist == NULL || elistsz <= 0) {
		return NULL;
	}

	/* allocate */
	identd = (mbfl_encoding_detector*)mbfl_malloc(sizeof(mbfl_encoding_detector));
	if (identd == NULL) {
		return NULL;
	}
	identd->filter_list = (mbfl_identify_filter **)mbfl_calloc(elistsz, sizeof(mbfl_identify_filter *));
	if (identd->filter_list == NULL) {
		mbfl_free(identd);
		return NULL;
	}

	/* create filters */
	i = 0;
	num = 0;
	while (i < elistsz) {
		filter = mbfl_identify_filter_new2(elist[i]);
		if (filter != NULL) {
			identd->filter_list[num] = filter;
			num++;
		}
		i++;
	}
	identd->filter_list_size = num;

	/* set strict flag */
	identd->strict = strict;

	return identd;
}