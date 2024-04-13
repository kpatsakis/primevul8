void fs_private_dir_list(const char *private_dir, const char *private_run_dir, const char *private_list) {
	timetrace_start();
	fs_private_dir_copy(private_dir, private_run_dir, private_list);
	fs_private_dir_mount(private_dir, private_run_dir);
	fmessage("Private %s installed in %0.2f ms\n", private_dir, timetrace_end());
}