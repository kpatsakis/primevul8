void rw_curr_act_stats(int ifd, int *curr, long *cnt, int *eosaf,
		       unsigned int act_id, int *reset, struct file_activity *file_actlst,
		       struct tm *rectime, char *file,
		       struct file_magic *file_magic)
{
	int rtype;
	int next, reset_cd;

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	if (DISPLAY_FIELD_LIST(fmt[f_position]->options)) {
		/* Print field list */
		list_fields(act_id);
	}

	/*
	 * Restore the first stats collected.
	 * Used to compute the rate displayed on the first line.
	 */
	copy_structures(act, id_seq, record_hdr, !*curr, 2);

	*cnt  = count;
	reset_cd = 1;

	do {
		/* Display <count> lines of stats */
		*eosaf = read_next_sample(ifd, IGNORE_RESTART | DONT_READ_CPU_NR,
					  *curr, file, &rtype, 0, file_magic,
					  file_actlst, rectime, UEOF_STOP);

		if (!*eosaf && (rtype != R_RESTART) && (rtype != R_COMMENT)) {
			next = generic_write_stats(*curr, tm_start.use, tm_end.use, *reset, cnt,
						   NULL, rectime, reset_cd, act_id);
			reset_cd = 0;

			if (next) {
				/*
				 * next is set to 1 when we were close enough to desired interval.
				 * In this case, the call to generic_write_stats() has actually
				 * displayed a line of stats.
				 */
				*curr ^= 1;
				if (*cnt > 0) {
					(*cnt)--;
				}
			}
			*reset = FALSE;
		}
	}
	while (*cnt && !*eosaf && (rtype != R_RESTART));

	*reset = TRUE;
}