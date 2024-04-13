static void duplicate(const char *fname, const char *private_dir, const char *private_run_dir) {
	assert(fname);

	if (*fname == '~' || *fname == '/' || strstr(fname, "..")) {
		fprintf(stderr, "Error: \"%s\" is an invalid filename\n", fname);
		exit(1);
	}
	invalid_filename(fname, 0); // no globbing

	char *src;
	if (asprintf(&src,  "%s/%s", private_dir, fname) == -1)
		errExit("asprintf");
	if (check_dir_or_file(src) == 0) {
		fwarning("skipping %s for private %s\n", fname, private_dir);
		free(src);
		return;
	}

	if (arg_debug)
		printf("Copying %s to private %s\n", src, private_dir);

	char *dst;
	if (asprintf(&dst, "%s/%s", private_run_dir, fname) == -1)
		errExit("asprintf");

	build_dirs(src, dst, strlen(private_dir), strlen(private_run_dir));

	// follow links! this will make a copy of the file or directory pointed by the symlink
	// this will solve problems such as NixOS #4887
	// don't follow links to dynamic directories such as /proc
	if (strcmp(src, "/etc/mtab") == 0)
		sbox_run(SBOX_ROOT | SBOX_SECCOMP, 3, PATH_FCOPY, src, dst);
	else
		sbox_run(SBOX_ROOT | SBOX_SECCOMP, 4, PATH_FCOPY, "--follow-link", src, dst);

	free(dst);
	fs_logger2("clone", src);
	free(src);
}