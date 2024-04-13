void logic1_display_loop(int ifd, struct file_activity *file_actlst, char *file,
			 struct file_magic *file_magic,
			 struct tm *rectime, struct tm *loctime)
{
	int curr, tab = 0, rtype;
	int eosaf, next, reset = FALSE;
	long cnt = 1;

	if (format == F_JSON_OUTPUT) {
		/* Use a decimal point to make JSON code compliant with RFC7159 */
		setlocale(LC_NUMERIC, "C");
	}

	/* Save current file position */
	seek_file_position(ifd, DO_SAVE);

	/* Print header (eg. XML file header) */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&tab, F_BEGIN, file, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}

	/* Process activities */
	if (*fmt[f_position]->f_statistics) {
		(*fmt[f_position]->f_statistics)(&tab, F_BEGIN);
	}

	do {
		/*
		 * If this record is a special (RESTART or COMMENT) one,
		 * skip it and try to read the next record in file.
		 */
		do {
			eosaf = read_next_sample(ifd, IGNORE_COMMENT | IGNORE_RESTART, 0,
						 file, &rtype, tab, file_magic, file_actlst,
						 rectime, loctime, UEOF_STOP);
		}
		while (!eosaf && ((rtype == R_RESTART) || (rtype == R_COMMENT) ||
			(tm_start.use && (datecmp(loctime, &tm_start) < 0)) ||
			(tm_end.use && (datecmp(loctime, &tm_end) >= 0))));

		/* Save the first stats collected. Used for example in next_slice() function */
		copy_structures(act, id_seq, record_hdr, 2, 0);

		curr = 1;
		cnt = count;
		reset = TRUE;

		if (!eosaf) {
			do {
				eosaf = read_next_sample(ifd, IGNORE_COMMENT | IGNORE_RESTART, curr,
							 file, &rtype, tab, file_magic, file_actlst,
							 rectime, loctime, UEOF_CONT);

				if (!eosaf && (rtype != R_COMMENT) && (rtype != R_RESTART)) {
					if (*fmt[f_position]->f_statistics) {
						(*fmt[f_position]->f_statistics)(&tab, F_MAIN);
					}

					/* next is set to 1 when we were close enough to desired interval */
					next = generic_write_stats(curr, tm_start.use, tm_end.use, reset,
								  &cnt, &tab, rectime, loctime,
								  FALSE, ALL_ACTIVITIES);

					if (next) {
						curr ^= 1;
						if (cnt > 0) {
							cnt--;
						}
					}
					reset = FALSE;
				}
			}
			while (cnt && !eosaf && (rtype != R_RESTART));

			if (!cnt) {
				/* Go to next Linux restart, if possible */
				do {
					eosaf = read_next_sample(ifd, IGNORE_COMMENT | IGNORE_RESTART, curr,
								 file, &rtype, tab, file_magic, file_actlst,
								 rectime, loctime, UEOF_CONT);
				}
				while (!eosaf && (rtype != R_RESTART));
			}
			reset = TRUE;
		}
	}
	while (!eosaf);

	if (*fmt[f_position]->f_statistics) {
		(*fmt[f_position]->f_statistics)(&tab, F_END);
	}

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	/* Process now RESTART entries to display restart messages */
	if (*fmt[f_position]->f_restart) {
		(*fmt[f_position]->f_restart)(&tab, F_BEGIN, NULL, NULL, FALSE,
					      &file_hdr);
	}

	do {
		eosaf = read_next_sample(ifd, IGNORE_COMMENT, 0,
					 file, &rtype, tab, file_magic, file_actlst,
					 rectime, loctime, UEOF_CONT);
	}
	while (!eosaf);

	if (*fmt[f_position]->f_restart) {
		(*fmt[f_position]->f_restart)(&tab, F_END, NULL, NULL, FALSE, &file_hdr);
	}

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	/* Last, process COMMENT entries to display comments */
	if (DISPLAY_COMMENT(flags)) {
		if (*fmt[f_position]->f_comment) {
			(*fmt[f_position]->f_comment)(&tab, F_BEGIN, NULL, NULL, 0, NULL,
						      &file_hdr);
		}
		do {
			eosaf = read_next_sample(ifd, IGNORE_RESTART, 0,
						 file, &rtype, tab, file_magic, file_actlst,
						 rectime, loctime, UEOF_CONT);
		}
		while (!eosaf);

		if (*fmt[f_position]->f_comment) {
			(*fmt[f_position]->f_comment)(&tab, F_END, NULL, NULL, 0, NULL,
						      &file_hdr);
		}
	}

	/* Print header trailer */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&tab, F_END, file, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}
}