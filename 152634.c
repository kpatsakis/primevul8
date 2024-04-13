void logic2_display_loop(int ifd, struct file_activity *file_actlst,
			 struct tm *rectime, struct tm *loctime, char *file,
			 struct file_magic *file_magic)
{
	int i, p;
	int curr = 1, rtype;
	int eosaf = TRUE, reset = FALSE;
	long cnt = 1;

	/* Read system statistics from file */
	do {
		/*
		 * If this record is a special (RESTART or COMMENT) one, print it and
		 * (try to) get another one.
		 */
		do {
			if (read_next_sample(ifd, IGNORE_NOTHING, 0,
					     file, &rtype, 0, file_magic, file_actlst,
					     rectime, loctime, UEOF_STOP))
				/* End of sa data file */
				return;
		}
		while ((rtype == R_RESTART) || (rtype == R_COMMENT) ||
		       (tm_start.use && (datecmp(loctime, &tm_start) < 0)) ||
		       (tm_end.use && (datecmp(loctime, &tm_end) >= 0)));

		/* Save the first stats collected. Used for example in next_slice() function */
		copy_structures(act, id_seq, record_hdr, 2, 0);

		/* Set flag to reset last_uptime variable. Should be done after a LINUX RESTART record */
		reset = TRUE;

		/* Save current file position */
		seek_file_position(ifd, DO_SAVE);

		/* Read and write stats located between two possible Linux restarts */

		if (DISPLAY_HORIZONTALLY(flags)) {
			/*
			 * If stats are displayed horizontally, then all activities
			 * are printed on the same line.
			 */
			rw_curr_act_stats(ifd, &curr, &cnt, &eosaf,
					  ALL_ACTIVITIES, &reset, file_actlst,
					  rectime, loctime, file, file_magic);
		}
		else {
			/* For each requested activity... */
			for (i = 0; i < NR_ACT; i++) {

				if (!id_seq[i])
					continue;

				p = get_activity_position(act, id_seq[i], EXIT_IF_NOT_FOUND);
				if (!IS_SELECTED(act[p]->options))
					continue;

				if (!HAS_MULTIPLE_OUTPUTS(act[p]->options)) {
					rw_curr_act_stats(ifd, &curr, &cnt, &eosaf,
							  act[p]->id, &reset, file_actlst,
							  rectime, loctime, file, file_magic);
				}
				else {
					unsigned int optf, msk;

					optf = act[p]->opt_flags;

					for (msk = 1; msk < 0x100; msk <<= 1) {
						if ((act[p]->opt_flags & 0xff) & msk) {
							act[p]->opt_flags &= (0xffffff00 + msk);

							rw_curr_act_stats(ifd, &curr, &cnt, &eosaf,
									  act[p]->id, &reset, file_actlst,
									  rectime, loctime, file,
									  file_magic);
							act[p]->opt_flags = optf;
						}
					}
				}
			}
		}

		if (!cnt) {
			/* Go to next Linux restart, if possible */
			do {
				eosaf = read_next_sample(ifd, IGNORE_RESTART | DONT_READ_CPU_NR,
							 curr, file, &rtype, 0, file_magic,
							 file_actlst, rectime, loctime, UEOF_STOP);
			}
			while (!eosaf && (rtype != R_RESTART));
		}

		/*
		 * The last record we read was a RESTART one: Print it.
		 * NB: Unlike COMMENTS records (which are displayed for each
		 * activity), RESTART ones are only displayed once.
		 */
		if (!eosaf && (record_hdr[curr].record_type == R_RESTART)) {
			print_special_record(&record_hdr[curr], flags, &tm_start, &tm_end,
					     R_RESTART, ifd, rectime, loctime, file, 0,
					     file_magic, &file_hdr, act, fmt[f_position],
					     endian_mismatch, arch_64);
		}
	}
	while (!eosaf);
}