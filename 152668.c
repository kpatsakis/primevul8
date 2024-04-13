void display_curr_act_graphs(int ifd, int *curr, long *cnt, int *eosaf,
			     int p, int *reset, struct file_activity *file_actlst,
			     struct tm *rectime, struct tm *loctime,
			     char *file, struct file_magic *file_magic, int *g_nr,
			     int nr_act_dispd)
{
	struct svg_parm parm;
	int rtype;
	int next, reset_cd;

	/* Rewind file */
	seek_file_position(ifd, DO_RESTORE);

	/*
	 * Restore the first stats collected.
	 * Used to compute the rate displayed on the first line.
	 */
	copy_structures(act, id_seq, record_hdr, !*curr, 2);

	parm.graph_no = *g_nr;
	parm.ust_time_ref = (unsigned long long) get_time_ref();
	parm.ust_time_first = record_hdr[2].ust_time;
	parm.restart = TRUE;
	parm.file_hdr = &file_hdr;
	parm.nr_act_dispd = nr_act_dispd;

	*cnt  = count;
	reset_cd = 1;

	/* Allocate graphs arrays */
	(*act[p]->f_svg_print)(act[p], !*curr, F_BEGIN, &parm, 0, &record_hdr[!*curr]);

	do {
		*eosaf = read_next_sample(ifd, IGNORE_RESTART | IGNORE_COMMENT | SET_TIMESTAMPS,
					  *curr, file, &rtype, 0, file_magic,
					  file_actlst, rectime, loctime, UEOF_CONT);

		if (!*eosaf && (rtype != R_COMMENT) && (rtype != R_RESTART)) {

			next = generic_write_stats(*curr, tm_start.use, tm_end.use, *reset, cnt,
						   &parm, rectime, loctime, reset_cd, act[p]->id);
			reset_cd = 0;
			if (next) {
				/*
				 * next is set to 1 when we were close enough to desired interval.
				 * In this case, the call to generic_write_stats() has actually
				 * displayed a line of stats.
				 */
				parm.restart = FALSE;
				parm.ust_time_end = record_hdr[*curr].ust_time;
				*curr ^= 1;
				if (*cnt > 0) {
					(*cnt)--;
				}
			}
			*reset = FALSE;
		}
		if (!*eosaf && (rtype == R_RESTART)) {
			parm.restart = TRUE;
			*reset = TRUE;
			/* Go to next statistics record, if possible */
			do {
				*eosaf = read_next_sample(ifd, IGNORE_RESTART | IGNORE_COMMENT | SET_TIMESTAMPS,
							  *curr, file, &rtype, 0, file_magic,
							  file_actlst, rectime, loctime, UEOF_CONT);
			}
			while (!*eosaf && ((rtype == R_RESTART) || (rtype == R_COMMENT)));

			*curr ^= 1;
		}
	}
	while (!*eosaf);

	*reset = TRUE;

	/* Determine X axis end value */
	if (DISPLAY_ONE_DAY(flags) &&
	    (parm.ust_time_ref + (3600 * 24) > parm.ust_time_end)) {
		parm.ust_time_end = parm.ust_time_ref + (3600 * 24);
	}

	/* Actually display graphs for current activity */
	(*act[p]->f_svg_print)(act[p], *curr, F_END, &parm, 0, &record_hdr[!*curr]);

	/* Update total number of graphs already displayed */
	*g_nr = parm.graph_no;
}