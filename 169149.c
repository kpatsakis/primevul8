int main(int argc, char **argv)
{
	int i, rc, opt = 1, args_idx = 1, p, q;
	int fd[2];
	int day_offset = 0;
	char from_file[MAX_FILE_LEN], to_file[MAX_FILE_LEN];
	char ltemp[1024];

	/* Compute page shift in kB */
	get_kb_shift();

	from_file[0] = to_file[0] = '\0';

#ifdef USE_NLS
	/* Init National Language Support */
	init_nls();
#endif

	/* Init color strings */
	init_colors();

	tm_start.use = tm_end.use = FALSE;

	/* Allocate and init activity bitmaps */
	allocate_bitmaps(act);

	init_structures();

	/* Process options */
	while (opt < argc) {

		if (!strcmp(argv[opt], "--sadc")) {
			/* Locate sadc */
			which_sadc();
		}

		else if (!strncmp(argv[opt], "--dev=", 6)) {
			/* Parse devices entered on the command line */
			p = get_activity_position(act, A_DISK, EXIT_IF_NOT_FOUND);
			parse_sa_devices(argv[opt], act[p], MAX_DEV_LEN, &opt, 6);
		}

		else if (!strncmp(argv[opt], "--fs=", 5)) {
			/* Parse devices entered on the command line */
			p = get_activity_position(act, A_FS, EXIT_IF_NOT_FOUND);
			parse_sa_devices(argv[opt], act[p], MAX_FS_LEN, &opt, 5);
		}

		else if (!strncmp(argv[opt], "--iface=", 8)) {
			/* Parse devices entered on the command line */
			p = get_activity_position(act, A_NET_DEV, EXIT_IF_NOT_FOUND);
			parse_sa_devices(argv[opt], act[p], MAX_IFACE_LEN, &opt, 8);
			q = get_activity_position(act, A_NET_EDEV, EXIT_IF_NOT_FOUND);
			act[q]->item_list = act[p]->item_list;
			act[q]->item_list_sz = act[p]->item_list_sz;
			act[q]->options |= AO_LIST_ON_CMDLINE;
		}

		else if (!strcmp(argv[opt], "--help")) {
			/* Display help message */
			display_help(argv[0]);
		}

		else if (!strcmp(argv[opt], "--human")) {
			/* Display sizes in a human readable format */
			flags |= S_F_UNIT;
			opt++;
		}

		else if (!strncmp(argv[opt], "--dec=", 6) && (strlen(argv[opt]) == 7)) {
			/* Get number of decimal places */
			dplaces_nr = atoi(argv[opt] + 6);
			if ((dplaces_nr < 0) || (dplaces_nr > 2)) {
				usage(argv[0]);
			}
			opt++;
		}

		else if (!strcmp(argv[opt], "-I")) {
			/* Parse -I option */
			if (parse_sar_I_opt(argv, &opt, &flags, act)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-D")) {
			/* Option to tell sar to write to saYYYYMMDD data files */
			flags |= S_F_SA_YYYYMMDD;
			opt++;
		}

		else if (!strcmp(argv[opt], "-P")) {
			/* Parse -P option */
			if (parse_sa_P_opt(argv, &opt, &flags, act)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-o")) {
			if (to_file[0]) {
				/* Output file already specified */
				usage(argv[0]);
			}
			/* Save stats to a file */
			if ((argv[++opt]) && strncmp(argv[opt], "-", 1) &&
			    (strspn(argv[opt], DIGITS) != strlen(argv[opt]))) {
				strncpy(to_file, argv[opt++], MAX_FILE_LEN);
				to_file[MAX_FILE_LEN - 1] = '\0';
			}
			else {
				strcpy(to_file, "-");
			}
		}

		else if (!strcmp(argv[opt], "-f")) {
			if (from_file[0] || day_offset) {
				/* Input file already specified */
				usage(argv[0]);
			}
			/* Read stats from a file */
			if ((argv[++opt]) && strncmp(argv[opt], "-", 1) &&
			    (strspn(argv[opt], DIGITS) != strlen(argv[opt]))) {
				strncpy(from_file, argv[opt++], MAX_FILE_LEN);
				from_file[MAX_FILE_LEN - 1] = '\0';
				/* Check if this is an alternate directory for sa files */
				check_alt_sa_dir(from_file, day_offset, -1);
			}
			else {
				set_default_file(from_file, day_offset, -1);
			}
		}

		else if (!strcmp(argv[opt], "-s")) {
			/* Get time start */
			if (parse_timestamp(argv, &opt, &tm_start, DEF_TMSTART)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-e")) {
			/* Get time end */
			if (parse_timestamp(argv, &opt, &tm_end, DEF_TMEND)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-i")) {
			if (!argv[++opt] || (strspn(argv[opt], DIGITS) != strlen(argv[opt]))) {
				usage(argv[0]);
			}
			interval = atol(argv[opt++]);
			if (interval < 1) {
				usage(argv[0]);
			}
			flags |= S_F_INTERVAL_SET;
		}

		else if (!strcmp(argv[opt], "-m")) {
			if (!argv[++opt]) {
				usage(argv[0]);
			}
			/* Parse option -m */
			if (parse_sar_m_opt(argv, &opt, act)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-n")) {
			if (!argv[++opt]) {
				usage(argv[0]);
			}
			/* Parse option -n */
			if (parse_sar_n_opt(argv, &opt, act)) {
				usage(argv[0]);
			}
		}

#ifdef TEST
		else if (!strncmp(argv[opt], "--unix_time=", 12)) {
			if (strspn(argv[opt] + 12, DIGITS) != strlen(argv[opt] + 12)) {
				usage(argv[0]);
			}
			__unix_time = atoll(argv[opt++] + 12);
		}
#endif
		else if ((strlen(argv[opt]) > 1) &&
			 (strlen(argv[opt]) < 4) &&
			 !strncmp(argv[opt], "-", 1) &&
			 (strspn(argv[opt] + 1, DIGITS) == (strlen(argv[opt]) - 1))) {
			if (from_file[0] || day_offset) {
				/* Input file already specified */
				usage(argv[0]);
			}
			day_offset = atoi(argv[opt++] + 1);
		}

		else if (!strncmp(argv[opt], "-", 1)) {
			/* Other options not previously tested */
			if ((rc = parse_sar_opt(argv, &opt, act, &flags, C_SAR)) != 0) {
				if (rc == 1) {
					usage(argv[0]);
				}
				exit(1);
			}
			opt++;
		}

		else if (interval < 0) {
			/* Get interval */
			if (strspn(argv[opt], DIGITS) != strlen(argv[opt])) {
				usage(argv[0]);
			}
			interval = atol(argv[opt++]);
			if (interval < 0) {
				usage(argv[0]);
			}
		}

		else {
			/* Get count value */
			if ((strspn(argv[opt], DIGITS) != strlen(argv[opt])) ||
			    !interval) {
				usage(argv[0]);
			}
			if (count) {
				/* Count parameter already set */
				usage(argv[0]);
			}
			count = atol(argv[opt++]);
			if (count < 1) {
				usage(argv[0]);
			}
		}
	}

	/* 'sar' is equivalent to 'sar -f' */
	if ((argc == 1) ||
	    (((interval < 0) || INTERVAL_SET(flags)) && !from_file[0] && !to_file[0])) {
		set_default_file(from_file, day_offset, -1);
	}

	if (tm_start.use && tm_end.use && (tm_end.tm_hour < tm_start.tm_hour)) {
		tm_end.tm_hour += 24;
	}

	/*
	 * Check option dependencies.
	 */
	/* You read from a file OR you write to it... */
	if (from_file[0] && to_file[0]) {
		fprintf(stderr, _("-f and -o options are mutually exclusive\n"));
		exit(1);
	}
	if (USE_OPTION_A(flags)) {
		/* Set -P ALL -I ALL if needed */
		set_bitmaps(act, &flags);
	}
	/* Use time start or option -i only when reading stats from a file */
	if ((tm_start.use || INTERVAL_SET(flags)) && !from_file[0]) {
		fprintf(stderr,
			_("Not reading from a system activity file (use -f option)\n"));
		exit(1);
	}
	/* Don't print stats since boot time if -o or -f options are used */
	if (!interval && (from_file[0] || to_file[0])) {
		usage(argv[0]);
	}

	/* Cannot enter a day shift with -o option */
	if (to_file[0] && day_offset) {
		usage(argv[0]);
	}

	if (USE_PRETTY_OPTION(flags)) {
		dm_major = get_devmap_major();
	}

	if (!count) {
		/*
		 * count parameter not set: Display all the contents of the file
		 * or generate a report continuously.
		 */
		count = -1;
	}

	/* Default is CPU activity... */
	select_default_activity(act);

	/* Check S_TIME_FORMAT variable contents */
	if (!is_iso_time_fmt())
		flags |= S_F_PREFD_TIME_OUTPUT;

	/* Reading stats from file: */
	if (from_file[0]) {
		if (interval < 0) {
			interval = 1;
		}

		/* Read stats from file */
		read_stats_from_file(from_file);

		/* Free stuctures and activity bitmaps */
		free_bitmaps(act);
		free_structures(act);

		return 0;
	}

	/* Reading stats from sadc: */

	/* Create anonymous pipe */
	if (pipe(fd) == -1) {
		perror("pipe");
		exit(4);
	}

	switch (fork()) {

	case -1:
		perror("fork");
		exit(4);
		break;

	case 0: /* Child */
		if (dup2(fd[1], STDOUT_FILENO) < 0) {
			perror("dup2");
			exit(4);
		}
		CLOSE_ALL(fd);

		/*
		 * Prepare options for sadc.
		 */
		/* Program name */
		salloc(0, SADC);

		/* Interval value */
		if (interval < 0) {
			usage(argv[0]);
		}
		else if (!interval) {
			strcpy(ltemp, "1");
			/*
			 * Display stats since system startup: Set <interval> to 1.
			 * <count> arg will also be set to 1 below.
			 */
			salloc(args_idx++, ltemp);
		}
		else {
			sprintf(ltemp, "%ld", interval);
		}
		salloc(args_idx++, ltemp);

		/* Count number */
		if (count >= 0) {
			sprintf(ltemp, "%ld", count + 1);
			salloc(args_idx++, ltemp);
		}

#ifdef TEST
		if (__unix_time) {
			sprintf(ltemp, "--unix_time=%ld", __unix_time);
			salloc(args_idx++, ltemp);
		}
#endif
		/* Flags to be passed to sadc */
		salloc(args_idx++, "-Z");

		/* Writing data to a file (option -o) */
		if (to_file[0]) {
			/* Set option -D if entered */
			if (USE_SA_YYYYMMDD(flags)) {
				salloc(args_idx++, "-D");
			}
			/* Collect all possible activities (option -S XALL for sadc) */
			salloc(args_idx++, "-S");
			salloc(args_idx++, K_XALL);
			/* Outfile arg */
			salloc(args_idx++, to_file);
		}
		else {
			/*
			 * If option -o hasn't been used, then tell sadc
			 * to collect only activities that will be displayed.
			 */
			salloc(args_idx++, "-S");
			strcpy(ltemp, K_A_NULL);
			for (i = 0; i < NR_ACT; i++) {
				if (IS_SELECTED(act[i]->options)) {
					strcat(ltemp, ",");
					strcat(ltemp, act[i]->name);
				}
			}
			salloc(args_idx++, ltemp);
		}

		/* Last arg is NULL */
		args[args_idx] = NULL;

		/* Call now the data collector */
#ifdef DEBUG
		fprintf(stderr, "%s: 1.sadc: %s\n", __FUNCTION__, SADC_PATH);
#endif

		execv(SADC_PATH, args);
#ifdef DEBUG
		fprintf(stderr, "%s: 2.sadc: %s\n", __FUNCTION__, SADC);
#endif
		execvp(SADC, args);
		/*
		 * Note: Don't use execl/execlp since we don't have a fixed number of
		 * args to give to sadc.
		 */
		fprintf(stderr, _("Cannot find the data collector (%s)\n"), SADC);
		perror("exec");
		exit(4);
		break;

	default: /* Parent */
		if (dup2(fd[0], STDIN_FILENO) < 0) {
			perror("dup2");
			exit(4);
		}
		CLOSE_ALL(fd);

		/* Get now the statistics */
		read_stats();

		break;
	}

	/* Free structures and activity bitmaps */
	free_bitmaps(act);
	free_structures(act);

	return 0;
}