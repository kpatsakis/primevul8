void logic1_display_loop(int ifd, char *file, struct file_activity *file_actlst,
			 struct file_magic *file_magic, struct tm *rectime, void *dparm)
{
	int curr, rtype, tab = 0;
	int eosaf, next, reset = FALSE;
	int ign_flag = IGNORE_COMMENT + IGNORE_RESTART;
	long cnt = 1;
	char *pcparchive = (char *) dparm;

	if (CREATE_ITEM_LIST(fmt[f_position]->options)) {
		/* Count items in file (e.g. for PCP output) */
		if (!count_file_items(ifd, file, file_magic, file_actlst, rectime))
			/* No record to display */
			return;
	}
	/* Save current file position */
	seek_file_position(ifd, DO_SAVE);

	/* Print header (eg. XML file header) */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&tab, F_BEGIN, pcparchive, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}

	if (ORDER_ALL_RECORDS(fmt[f_position]->options)) {
		ign_flag = IGNORE_NOTHING;

		/* RESTART and COMMENTS records will be immediately processed */
		if (*fmt[f_position]->f_restart) {
			(*fmt[f_position]->f_restart)(&tab, F_BEGIN, NULL, NULL, FALSE,
						      &file_hdr, NULL);
		}
		if (DISPLAY_COMMENT(flags) && (*fmt[f_position]->f_comment)) {
			(*fmt[f_position]->f_comment)(&tab, F_BEGIN, NULL, NULL, 0, NULL,
						      &file_hdr, NULL);
		}
	}

	/* Process activities */
	if (*fmt[f_position]->f_statistics) {
		(*fmt[f_position]->f_statistics)(&tab, F_BEGIN, act, id_seq);
	}

	do {
		/*
		 * If this record is a special (RESTART or COMMENT) one,
		 * process it then try to read the next record in file.
		 */
		do {
			eosaf = read_next_sample(ifd, ign_flag, 0, file,
						 &rtype, tab, file_magic, file_actlst,
						 rectime, UEOF_STOP);
		}
		while (!eosaf && ((rtype == R_RESTART) || (rtype == R_COMMENT) ||
			(tm_start.use && (datecmp(rectime, &tm_start, FALSE) < 0)) ||
			(tm_end.use && (datecmp(rectime, &tm_end, FALSE) >= 0))));

		curr = 1;
		cnt = count;
		reset = TRUE;

		if (!eosaf) {

			/* Save the first stats collected. Used for example in next_slice() function */
			copy_structures(act, id_seq, record_hdr, 2, 0);

			do {
				eosaf = read_next_sample(ifd, ign_flag, curr, file,
							 &rtype, tab, file_magic, file_actlst,
							 rectime, UEOF_CONT);

				if (!eosaf && (rtype != R_COMMENT) && (rtype != R_RESTART)) {
					if (*fmt[f_position]->f_statistics) {
						(*fmt[f_position]->f_statistics)(&tab, F_MAIN, act, id_seq);
					}

					/* next is set to 1 when we were close enough to desired interval */
					next = generic_write_stats(curr, tm_start.use, tm_end.use, reset,
								  &cnt, &tab, rectime, FALSE, ALL_ACTIVITIES);

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
					eosaf = read_next_sample(ifd, ign_flag, curr, file,
								 &rtype, tab, file_magic, file_actlst,
								 rectime, UEOF_CONT);
				}
				while (!eosaf && (rtype != R_RESTART));
			}
			reset = TRUE;
		}
	}
	while (!eosaf);

	if (*fmt[f_position]->f_statistics) {
		(*fmt[f_position]->f_statistics)(&tab, F_END, act, id_seq);
	}

	if (ign_flag == IGNORE_NOTHING) {
		/*
		 * RESTART and COMMENT records have already been processed.
		 * Display possible trailing data then terminate.
		 */
		if (*fmt[f_position]->f_restart) {
			(*fmt[f_position]->f_restart)(&tab, F_END, NULL, NULL,
						      FALSE, &file_hdr, NULL);
		}
		if (DISPLAY_COMMENT(flags) && (*fmt[f_position]->f_comment)) {
			(*fmt[f_position]->f_comment)(&tab, F_END, NULL, NULL, 0, NULL,
						      &file_hdr, NULL);
		}
		goto terminate;
	}

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	/* Process now RESTART entries to display restart messages */
	if (*fmt[f_position]->f_restart) {
		(*fmt[f_position]->f_restart)(&tab, F_BEGIN, NULL, NULL, FALSE,
					      &file_hdr, NULL);
	}

	do {
		eosaf = read_next_sample(ifd, IGNORE_COMMENT, 0,
					 file, &rtype, tab, file_magic, file_actlst,
					 rectime, UEOF_CONT);
	}
	while (!eosaf);

	if (*fmt[f_position]->f_restart) {
		(*fmt[f_position]->f_restart)(&tab, F_END, NULL, NULL, FALSE, &file_hdr, NULL);
	}

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	/* Last, process COMMENT entries to display comments */
	if (DISPLAY_COMMENT(flags)) {
		if (*fmt[f_position]->f_comment) {
			(*fmt[f_position]->f_comment)(&tab, F_BEGIN, NULL, NULL, 0, NULL,
						      &file_hdr, NULL);
		}
		do {
			eosaf = read_next_sample(ifd, IGNORE_RESTART, 0,
						 file, &rtype, tab, file_magic, file_actlst,
						 rectime, UEOF_CONT);
		}
		while (!eosaf);

		if (*fmt[f_position]->f_comment) {
			(*fmt[f_position]->f_comment)(&tab, F_END, NULL, NULL, 0, NULL,
						      &file_hdr, NULL);
		}
	}

terminate:
	/* Print header trailer */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&tab, F_END, pcparchive, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}
}