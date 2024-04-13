static int checkout_submodule_update_index(
	checkout_data *data,
	const git_diff_file *file)
{
	git_buf *fullpath;
	struct stat st;

	/* update the index unless prevented */
	if ((data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) != 0)
		return 0;

	if (checkout_target_fullpath(&fullpath, data, file->path) < 0)
		return -1;

	data->perfdata.stat_calls++;
	if (p_stat(fullpath->ptr, &st) < 0) {
		git_error_set(
			GIT_ERROR_CHECKOUT, "could not stat submodule %s\n", file->path);
		return GIT_ENOTFOUND;
	}

	st.st_mode = GIT_FILEMODE_COMMIT;

	return checkout_update_index(data, file, &st);
}