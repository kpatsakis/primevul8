int main(int argc, char **argv)
{
	int opt = 1, sar_options = 0;
	int day_offset = 0;
	int i, rc, p, q;
	char dfile[MAX_FILE_LEN], pcparchive[MAX_FILE_LEN];
	char *t, *v;

	/* Compute page shift in kB */
	get_kb_shift();

	dfile[0] = pcparchive[0] = '\0';

#ifdef USE_NLS
	/* Init National Language Support */
	init_nls();
#endif

	tm_start.use = tm_end.use = FALSE;

	/* Allocate and init activity bitmaps */
	allocate_bitmaps(act);

	/* Init some structures */
	init_structures();

	/* Process options */
	while (opt < argc) {

		if (!strcmp(argv[opt], "-I")) {
			if (!sar_options) {
				usage(argv[0]);
			}
			if (parse_sar_I_opt(argv, &opt, &flags, act)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-P")) {
			if (parse_sa_P_opt(argv, &opt, &flags, act)) {
				usage(argv[0]);
			}
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

		else if (!strcmp(argv[opt], "-O")) {
			/* Parse output options */
			if (!argv[++opt] || sar_options) {
				usage(argv[0]);
			}
			for (t = strtok(argv[opt], ","); t; t = strtok(NULL, ",")) {
				if (!strcmp(t, K_SKIP_EMPTY)) {
					flags |= S_F_SVG_SKIP;
				}
				else if (!strcmp(t, K_AUTOSCALE)) {
					flags |= S_F_SVG_AUTOSCALE;
				}
				else if (!strcmp(t, K_ONEDAY)) {
					flags |= S_F_SVG_ONE_DAY;
				}
				else if (!strcmp(t, K_SHOWIDLE)) {
					flags |= S_F_SVG_SHOW_IDLE;
				}
				else if (!strcmp(t, K_SHOWINFO)) {
					flags |= S_F_SVG_SHOW_INFO;
				}
				else if (!strcmp(t, K_DEBUG)) {
					flags |= S_F_RAW_DEBUG_MODE;
				}
				else if (!strncmp(t, K_HEIGHT, strlen(K_HEIGHT))) {
					v = t + strlen(K_HEIGHT);
					if (!strlen(v) || (strspn(v, DIGITS) != strlen(v))) {
						usage(argv[0]);
					}
					canvas_height = atoi(v);
					flags |= S_F_SVG_HEIGHT;
				}
				else if (!strcmp(t, K_PACKED)) {
					flags |= S_F_SVG_PACKED;
				}
				else if (!strcmp(t, K_SHOWTOC)) {
					flags |= S_F_SVG_SHOW_TOC;
				}
				else if (!strcmp(t, K_CUSTOMCOL)) {
					palette = SVG_CUSTOM_COL_PALETTE;
				}
				else if (!strcmp(t, K_BWCOL)) {
					palette = SVG_BW_COL_PALETTE;
				}
				else if (!strncmp(t, K_PCPARCHIVE, strlen(K_PCPARCHIVE))) {
					v = t + strlen(K_PCPARCHIVE);
					strncpy(pcparchive, v, MAX_FILE_LEN);
					pcparchive[MAX_FILE_LEN - 1] = '\0';
				}
				else {
					usage(argv[0]);
				}
			}
			opt++;
		}

		else if ((strlen(argv[opt]) > 1) &&
			 (strlen(argv[opt]) < 4) &&
			 !strncmp(argv[opt], "-", 1) &&
			 (strspn(argv[opt] + 1, DIGITS) == (strlen(argv[opt]) - 1))) {
			if (dfile[0] || day_offset) {
				/* File already specified */
				usage(argv[0]);
			}
			day_offset = atoi(argv[opt++] + 1);
		}

		else if (!strcmp(argv[opt], "--")) {
			sar_options = 1;
			opt++;
		}

		else if (!strcmp(argv[opt], "-m")) {
			if (!argv[++opt] || !sar_options) {
				usage(argv[0]);
			}
			/* Parse sar's option -m */
			if (parse_sar_m_opt(argv, &opt, act)) {
				usage(argv[0]);
			}
		}

		else if (!strcmp(argv[opt], "-n")) {
			if (!argv[++opt] || !sar_options) {
				usage(argv[0]);
			}
			/* Parse sar's option -n */
			if (parse_sar_n_opt(argv, &opt, act)) {
				usage(argv[0]);
			}
		}

		else if (!strncmp(argv[opt], "-", 1)) {
			/* Other options not previously tested */
			if (sar_options) {
				if ((rc = parse_sar_opt(argv, &opt, act, &flags, C_SADF)) != 0) {
					if (rc == 1) {
						usage(argv[0]);
					}
					exit(1);
				}
			}
			else {

				for (i = 1; *(argv[opt] + i); i++) {

					switch (*(argv[opt] + i)) {

					case 'C':
						flags |= S_F_COMMENT;
						break;

					case 'c':
						if (format) {
							usage(argv[0]);
						}
						format = F_CONV_OUTPUT;
						break;

					case 'd':
						if (format) {
							usage(argv[0]);
						}
						format = F_DB_OUTPUT;
						break;

					case 'g':
						if (format) {
							usage(argv[0]);
						}
						format = F_SVG_OUTPUT;
						break;

					case 'h':
						flags |= S_F_HORIZONTALLY;
						break;

					case 'H':
						flags |= S_F_HDR_ONLY;
						break;

					case 'j':
						if (format) {
							usage(argv[0]);
						}
						format = F_JSON_OUTPUT;
						break;

					case 'l':
						if (format) {
							usage(argv[0]);
						}
						format = F_PCP_OUTPUT;
						break;

					case 'p':
						if (format) {
							usage(argv[0]);
						}
						format = F_PPC_OUTPUT;
						break;

					case 'r':
						if (format) {
							usage(argv[0]);
						}
						format = F_RAW_OUTPUT;
						break;

					case 'T':
						flags |= S_F_LOCAL_TIME;
						break;

					case 't':
						flags |= S_F_TRUE_TIME;
						break;

					case 'U':
						flags |= S_F_SEC_EPOCH;
						break;

					case 'x':
						if (format) {
							usage(argv[0]);
						}
						format = F_XML_OUTPUT;
						break;

					case 'V':
						print_version();
						break;

					default:
						usage(argv[0]);
					}
				}
			}
			opt++;
		}

		/* Get data file name */
		else if (strspn(argv[opt], DIGITS) != strlen(argv[opt])) {
			if (dfile[0] || day_offset) {
				/* File already specified */
				usage(argv[0]);
			}
			/* Write data to file */
			strncpy(dfile, argv[opt++], MAX_FILE_LEN);
			dfile[MAX_FILE_LEN - 1] = '\0';
			/* Check if this is an alternate directory for sa files */
			check_alt_sa_dir(dfile, 0, -1);
		}

		else if (interval < 0) {
			/* Get interval */
			if (strspn(argv[opt], DIGITS) != strlen(argv[opt])) {
				usage(argv[0]);
			}
			interval = atol(argv[opt++]);
			if (interval <= 0) {
				usage(argv[0]);
			}
		}

		else {
			/* Get count value */
			if (strspn(argv[opt], DIGITS) != strlen(argv[opt])) {
				usage(argv[0]);
			}
			if (count) {
				/* Count parameter already set */
				usage(argv[0]);
			}
			count = atol(argv[opt++]);
			if (count < 0) {
				usage(argv[0]);
			}
			else if (!count) {
				count = -1;	/* To generate a report continuously */
			}
		}
	}

	if (USE_OPTION_A(flags)) {
		/* Set -P ALL -I ALL if needed */
		set_bitmaps(act, &flags);
	}

	/* sadf reads current daily data file by default */
	if (!dfile[0]) {
		set_default_file(dfile, day_offset, -1);
	}

	/* PCP mode: If no archive file specified then use the name of the daily data file */
	if (!pcparchive[0] && (format == F_PCP_OUTPUT)) {
		strcpy(pcparchive, dfile);
	}

#ifndef HAVE_PCP
	if (format == F_PCP_OUTPUT) {
		fprintf(stderr, _("PCP support not compiled in\n"));
		exit(1);
	}
#endif

	if (tm_start.use && tm_end.use && (tm_end.tm_hour < tm_start.tm_hour)) {
		tm_end.tm_hour += 24;
	}

	if (USE_PRETTY_OPTION(flags)) {
		dm_major = get_devmap_major();
	}

	/* Options -T, -t and -U are mutually exclusive */
	if ((PRINT_LOCAL_TIME(flags) + PRINT_TRUE_TIME(flags) +
	    PRINT_SEC_EPOCH(flags)) > 1) {
		usage(argv[0]);
	}

	/*
	 * Display all the contents of the daily data file if the count parameter
	 * was not set on the command line.
	 */
	if (!count) {
		count = -1;
	}

	/* Default is CPU activity */
	select_default_activity(act);

	/* Check options consistency with selected output format. Default is PPC display */
	check_format_options();

	if (interval < 0) {
		interval = 1;
	}

	if (format == F_CONV_OUTPUT) {
		/* Convert file to current format */
		convert_file(dfile, act);
	}
	else {
		/* Read stats from file */
		read_stats_from_file(dfile, pcparchive);
	}

	/* Free bitmaps */
	free_bitmaps(act);

	return 0;
}