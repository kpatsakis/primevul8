static int checkout_safe_for_update_only(
	checkout_data *data, const char *path, mode_t expected_mode)
{
	struct stat st;

	data->perfdata.stat_calls++;

	if (p_lstat(path, &st) < 0) {
		/* if doesn't exist, then no error and no update */
		if (errno == ENOENT || errno == ENOTDIR)
			return 0;

		/* otherwise, stat error and no update */
		git_error_set(GIT_ERROR_OS, "failed to stat '%s'", path);
		return -1;
	}

	/* only safe for update if this is the same type of file */
	if ((st.st_mode & ~0777) == (expected_mode & ~0777))
		return 1;

	return 0;
}