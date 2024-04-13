int count_file_items(int ifd, char *file, struct file_magic *file_magic,
		      struct file_activity *file_actlst, struct tm *rectime,
		      struct tm *loctime)
{
	int i, eosaf, rtype;

	/* Save current file position */
	seek_file_position(ifd, DO_SAVE);

	/* Init maximum number of items for each activity */
	for (i = 0; i < NR_ACT; i++) {
		if (!HAS_LIST_ON_CMDLINE(act[i]->options)) {
			act[i]->item_list_sz = 0;
		}
	}

	/* Look for the first record that will be displayed */
	do {
		eosaf = read_next_sample(ifd, IGNORE_RESTART | IGNORE_COMMENT | SET_TIMESTAMPS,
					 0, file, &rtype, 0, file_magic, file_actlst,
					 rectime, loctime, UEOF_CONT);
		if (eosaf)
			/* No record to display */
			return 0;
	}
	while ((tm_start.use && (datecmp(loctime, &tm_start) < 0)) ||
	       (tm_end.use && (datecmp(loctime, &tm_end) >= 0)));

	/*
	 * Read all the file and determine the maximum number
	 * of items for each activity.
	 */
	do {
		for (i = 0; i < NR_ACT; i++) {
			if (!HAS_LIST_ON_CMDLINE(act[i]->options)) {
				if (act[i]->f_count_new) {
					act[i]->item_list_sz += (*act[i]->f_count_new)(act[i], 0);
				}
				else if (act[i]->nr[0] > act[i]->item_list_sz) {
					act[i]->item_list_sz = act[i]->nr[0];
				}
			}
		}

		do {
			eosaf = read_next_sample(ifd, IGNORE_RESTART | IGNORE_COMMENT | SET_TIMESTAMPS,
						 0, file, &rtype, 0, file_magic, file_actlst,
						 rectime, loctime, UEOF_CONT);
			if (eosaf ||
			    (tm_end.use && (datecmp(loctime, &tm_end) >= 0)))
				/* End of data file or end time exceeded */
				break;
		}
		while ((rtype == R_RESTART) || (rtype == R_COMMENT));
	}
	while (!eosaf && !(tm_end.use && (datecmp(loctime, &tm_end) >= 0)));

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	return 1;
}