void fs_private_dir_copy(const char *private_dir, const char *private_run_dir, const char *private_list) {
	assert(private_dir);
	assert(private_run_dir);
	assert(private_list);

	// nothing to do if directory does not exist
	struct stat s;
	if (stat(private_dir, &s) == -1) {
		if (arg_debug)
			printf("Cannot find %s: %s\n", private_dir, strerror(errno));
		return;
	}

	// create /run/firejail/mnt/etc directory
	mkdir_attr(private_run_dir, 0755, 0, 0);
	selinux_relabel_path(private_run_dir, private_dir);
	fs_logger2("tmpfs", private_dir);

	fs_logger_print();	// save the current log


	// copy the list of files in the new etc directory
	// using a new child process with root privileges
	if (*private_list != '\0') {
		if (arg_debug)
			printf("Copying files in the new %s directory:\n", private_dir);

		// copy the list of files in the new home directory
		char *dlist = strdup(private_list);
		if (!dlist)
			errExit("strdup");


		char *ptr = strtok(dlist, ",");
		if (!ptr) {
			fprintf(stderr, "Error: invalid private %s argument\n", private_dir);
			exit(1);
		}
		duplicate(ptr, private_dir, private_run_dir);

		while ((ptr = strtok(NULL, ",")) != NULL)
			duplicate(ptr, private_dir, private_run_dir);
		free(dlist);
		fs_logger_print();
	}
}