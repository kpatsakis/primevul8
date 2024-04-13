void fs_private_dir_mount(const char *private_dir, const char *private_run_dir) {
	assert(private_dir);
	assert(private_run_dir);

	if (arg_debug)
		printf("Mount-bind %s on top of %s\n", private_run_dir, private_dir);

	// nothing to do if directory does not exist
	struct stat s;
	if (stat(private_dir, &s) == -1) {
		if (arg_debug)
			printf("Cannot find %s: %s\n", private_dir, strerror(errno));
		return;
	}

	if (mount(private_run_dir, private_dir, NULL, MS_BIND|MS_REC, NULL) < 0)
		errExit("mount bind");
	fs_logger2("mount", private_dir);

	// mask private_run_dir (who knows if there are writable paths, and it is mounted exec)
	if (mount("tmpfs", private_run_dir, "tmpfs", MS_NOSUID | MS_NODEV | MS_STRICTATIME,  "mode=755,gid=0") < 0)
		errExit("mounting tmpfs");
	fs_logger2("tmpfs", private_run_dir);
}