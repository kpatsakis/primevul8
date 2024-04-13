static int mkpath2file(
	checkout_data *data, const char *path, unsigned int mode)
{
	struct stat st;
	bool remove_existing = should_remove_existing(data);
	unsigned int flags =
		(remove_existing ? MKDIR_REMOVE_EXISTING : MKDIR_NORMAL) |
		GIT_MKDIR_SKIP_LAST;
	int error;

	if ((error = checkout_mkdir(
			data, path, data->opts.target_directory, mode, flags)) < 0)
		return error;

	if (remove_existing) {
		data->perfdata.stat_calls++;

		if (p_lstat(path, &st) == 0) {

			/* Some file, symlink or folder already exists at this name.
			 * We would have removed it in remove_the_old unless we're on
			 * a case inensitive filesystem (or the user has asked us not
			 * to).  Remove the similarly named file to write the new.
			 */
			error = git_futils_rmdir_r(path, NULL, GIT_RMDIR_REMOVE_FILES);
		} else if (errno != ENOENT) {
			git_error_set(GIT_ERROR_OS, "failed to stat '%s'", path);
			return GIT_EEXISTS;
		} else {
			git_error_clear();
		}
	}

	return error;
}