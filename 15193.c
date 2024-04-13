static int check_dir_or_file(const char *fname) {
	assert(fname);

	struct stat s;
	if (stat(fname, &s) == -1) {
		if (arg_debug)
			fwarning("file %s not found.\n", fname);
		return 0;
	}

	// read access
	if (access(fname, R_OK) == -1)
		goto errexit;

	// dir or regular file
	if (S_ISDIR(s.st_mode) || S_ISREG(s.st_mode) || !is_link(fname))
		return 1;	// normal exit

errexit:
	fprintf(stderr, "Error: invalid file type, %s.\n", fname);
	exit(1);
}