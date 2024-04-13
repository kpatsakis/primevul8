void read_stats_from_file(char dfile[], char pcparchive[])
{
	struct file_magic file_magic;
	struct file_activity *file_actlst = NULL;
	struct tm rectime;
	int ifd, ignore, tab = 0;

	/* Prepare file for reading and read its headers */
	ignore = ACCEPT_BAD_FILE_FORMAT(fmt[f_position]->options);
	check_file_actlst(&ifd, dfile, act, flags, &file_magic, &file_hdr,
			  &file_actlst, id_seq, ignore, &endian_mismatch, &arch_64);

	if (DISPLAY_HDR_ONLY(flags)) {
		if (*fmt[f_position]->f_header) {
			if (format == F_PCP_OUTPUT) {
				dfile = pcparchive;
			}
			/* Display only data file header then exit */
			(*fmt[f_position]->f_header)(&tab, F_BEGIN + F_END, dfile, &file_magic,
						     &file_hdr, act, id_seq, file_actlst);
		}
		exit(0);
	}

	/* Perform required allocations */
	allocate_structures(act);

	if (SET_LC_NUMERIC_C(fmt[f_position]->options)) {
		/* Use a decimal point */
		setlocale(LC_NUMERIC, "C");
	}

	/* Call function corresponding to selected output format */
	if (*fmt[f_position]->f_display) {
		(*fmt[f_position]->f_display)(ifd, dfile, file_actlst, &file_magic,
					      &rectime, pcparchive);
	}

	close(ifd);

	free(file_actlst);
	free_structures(act);
}