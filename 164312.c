mbfl_encoding_detector_feed(mbfl_encoding_detector *identd, mbfl_string *string)
{
	int i, n, num, bad, res;
	unsigned char *p;
	mbfl_identify_filter *filter;

	res = 0;
	/* feed data */
	if (identd != NULL && string != NULL && string->val != NULL) {
		num = identd->filter_list_size;
		n = string->len;
		p = string->val;
		bad = 0;
		while (n > 0) {
			for (i = 0; i < num; i++) {
				filter = identd->filter_list[i];
				if (!filter->flag) {
					(*filter->filter_function)(*p, filter);
					if (filter->flag) {
						bad++;
					}
				}
			}
			if ((num - 1) <= bad) {
				res = 1;
				break;
			}
			p++;
			n--;
		}
	}

	return res;
}