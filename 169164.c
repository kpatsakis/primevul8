void svg_display_loop(int ifd, char *file, struct file_activity *file_actlst,
		      struct file_magic *file_magic, struct tm *rectime, void *dparm)
{
	struct svg_hdr_parm parm;
	int i, p;
	int curr = 1, rtype, g_nr = 0, views_per_row = 1, nr_act_dispd;
	int eosaf = TRUE, reset = TRUE;
	long cnt = 1;
	int graph_nr = 0;

	/* Init custom colors palette */
	init_custom_color_palette();

	/*
	 * Calculate the number of rows and the max number of views per row to display.
	 * Result may be 0. In this case, "No data" will be displayed instead of the graphs.
	 */
	graph_nr = get_svg_graph_nr(ifd, file, file_magic,
				    file_actlst, rectime, &views_per_row, &nr_act_dispd);

	if (SET_CANVAS_HEIGHT(flags)) {
		/*
		 * Option "-O height=..." used: @graph_nr is NO LONGER a number
		 * of graphs but the SVG canvas height set on the command line.
		 */
		graph_nr = canvas_height;
	}


	parm.graph_nr = graph_nr;
	parm.views_per_row = PACK_VIEWS(flags) ? views_per_row : 1;
	parm.nr_act_dispd = nr_act_dispd;

	/* Print SVG header */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&parm, F_BEGIN + F_MAIN, file, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}

	/*
	* If this record is a special (RESTART or COMMENT) one, ignore it and
	* (try to) get another one.
	*/
	do {
		if (read_next_sample(ifd, IGNORE_RESTART | IGNORE_COMMENT, 0,
				     file, &rtype, 0, file_magic, file_actlst,
				     rectime, UEOF_CONT))
		{
			/* End of sa data file: No views displayed */
			parm.graph_nr = 0;
			goto close_svg;
		}
	}
	while ((rtype == R_RESTART) || (rtype == R_COMMENT) ||
	       (tm_start.use && (datecmp(rectime, &tm_start, FALSE) < 0)) ||
	       (tm_end.use && (datecmp(rectime, &tm_end, FALSE) >= 0)));

	/* Save the first stats collected. Used for example in next_slice() function */
	copy_structures(act, id_seq, record_hdr, 2, 0);

	/* Save current file position */
	seek_file_position(ifd, DO_SAVE);

	/* For each requested activity, display graphs */
	for (i = 0; i < NR_ACT; i++) {

		if (!id_seq[i])
			continue;

		p = get_activity_position(act, id_seq[i], EXIT_IF_NOT_FOUND);
		if (!IS_SELECTED(act[p]->options) || !act[p]->g_nr)
			continue;

		if (!HAS_MULTIPLE_OUTPUTS(act[p]->options)) {
			display_curr_act_graphs(ifd, &curr, &cnt, &eosaf,
						p, &reset, file_actlst,
						rectime, file,
						file_magic, &g_nr, nr_act_dispd);
		}
		else {
			unsigned int optf, msk;

			optf = act[p]->opt_flags;

			for (msk = 1; msk < 0x100; msk <<= 1) {
				if ((act[p]->opt_flags & 0xff) & msk) {
					act[p]->opt_flags &= (0xffffff00 + msk);
					display_curr_act_graphs(ifd, &curr, &cnt, &eosaf,
								p, &reset, file_actlst,
								rectime, file,
								file_magic, &g_nr, nr_act_dispd);
					act[p]->opt_flags = optf;
				}
			}
		}
	}

	/* Real number of graphs that have been displayed */
	parm.graph_nr = g_nr;

close_svg:
	/* Print SVG trailer */
	if (*fmt[f_position]->f_header) {
		(*fmt[f_position]->f_header)(&parm, F_END, file, file_magic,
					     &file_hdr, act, id_seq, file_actlst);
	}
}