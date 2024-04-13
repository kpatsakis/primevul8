void read_stats(void)
{
	int curr = 1;
	unsigned long lines;
	unsigned int rows;
	int dis_hdr = 0;

	/* Don't buffer data if redirected to a pipe... */
	setbuf(stdout, NULL);

	/* Read stats header */
	read_header_data();

	if (!get_activity_nr(act, AO_SELECTED, COUNT_ACTIVITIES)) {
		/* Requested activities not available: Exit */
		print_collect_error();
	}

	/* Determine if a stat line header has to be displayed */
	dis_hdr = check_line_hdr();

	lines = rows = get_win_height();

	/* Perform required allocations */
	allocate_structures(act);

	/* Print report header */
	print_report_hdr(flags, &rectime, &file_hdr);

	/* Read system statistics sent by the data collector */
	read_sadc_stat_bunch(0);

	if (!interval) {
		/* Display stats since boot time and exit */
		write_stats_startup(0);
	}

	/* Save the first stats collected. Will be used to compute the average */
	copy_structures(act, id_seq, record_hdr, 2, 0);

	/* Set a handler for SIGINT */
	memset(&int_act, 0, sizeof(int_act));
	int_act.sa_handler = int_handler;
	int_act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &int_act, NULL);

	/* Main loop */
	do {

		/* Get stats */
		read_sadc_stat_bunch(curr);
		if (sigint_caught) {
			/*
			 * SIGINT signal caught (it is sent by sadc).
			 * => Display average stats.
			 */
			curr ^= 1; /* No data retrieved from last read */
			break;
		}

		/* Print results */
		if (!dis_hdr) {
			dish = lines / rows;
			if (dish) {
				lines %= rows;
			}
			lines++;
		}
		write_stats(curr, USE_SADC, &count, NO_TM_START, tm_end.use,
			    NO_RESET, ALL_ACTIVITIES, TRUE);

		if (record_hdr[curr].record_type == R_LAST_STATS) {
			/* File rotation is happening: Re-read header data sent by sadc */
			read_header_data();
			allocate_structures(act);
		}

		if (count > 0) {
			count--;
		}
		if (count) {
			curr ^= 1;
		}
	}
	while (count);

	/*
	 * Print statistics average.
	 * At least one line of stats must have been displayed for this.
	 * (There may be no lines at all if we press Ctrl/C immediately).
	 */
	dish = dis_hdr;
	if (avg_count) {
		write_stats_avg(curr, USE_SADC, ALL_ACTIVITIES);
	}
}