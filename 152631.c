void read_stats_from_file(char dfile[])
{
	struct file_magic file_magic;
	struct file_activity *file_actlst = NULL;
	struct tm rectime, loctime;
	int ifd, ignore, tab = 0;

	/* Prepare file for reading and read its headers */
	ignore = ACCEPT_BAD_FILE_FORMAT(fmt[f_position]->options);
	check_file_actlst(&ifd, dfile, act, &file_magic, &file_hdr,
			  &file_actlst, id_seq, ignore, &endian_mismatch, &arch_64);

	if (DISPLAY_HDR_ONLY(flags)) {
		if (*fmt[f_position]->f_header) {
			/* Display only data file header then exit */
			(*fmt[f_position]->f_header)(&tab, F_BEGIN + F_END, dfile, &file_magic,
						     &file_hdr, act, id_seq, file_actlst);
		}
		exit(0);
	}

	/* Perform required allocations */
	allocate_structures(act);

	/* Call function corresponding to selected output format */
	if (format == F_SVG_OUTPUT) {
		logic3_display_loop(ifd, file_actlst,
				    &rectime, &loctime, dfile, &file_magic);
	}
	else if (DISPLAY_GROUPED_STATS(fmt[f_position]->options)) {
		logic2_display_loop(ifd, file_actlst,
				    &rectime, &loctime, dfile, &file_magic);
	}
	else {
		logic1_display_loop(ifd, file_actlst, dfile,
				    &file_magic, &rectime, &loctime);
	}

	close(ifd);

	free(file_actlst);
	free_structures(act);
}