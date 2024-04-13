static int checkout_remove_the_old(
	unsigned int *actions,
	checkout_data *data)
{
	int error = 0;
	git_diff_delta *delta;
	const char *str;
	size_t i;
	git_buf *fullpath;
	uint32_t flg = GIT_RMDIR_EMPTY_PARENTS |
		GIT_RMDIR_REMOVE_FILES | GIT_RMDIR_REMOVE_BLOCKERS;

	if (data->opts.checkout_strategy & GIT_CHECKOUT_SKIP_LOCKED_DIRECTORIES)
		flg |= GIT_RMDIR_SKIP_NONEMPTY;

	if (checkout_target_fullpath(&fullpath, data, NULL) < 0)
		return -1;

	git_vector_foreach(&data->diff->deltas, i, delta) {
		if (actions[i] & CHECKOUT_ACTION__REMOVE) {
			error = git_futils_rmdir_r(
				delta->old_file.path, fullpath->ptr, flg);

			if (error < 0)
				return error;

			data->completed_steps++;
			report_progress(data, delta->old_file.path);

			if ((actions[i] & CHECKOUT_ACTION__UPDATE_BLOB) == 0 &&
				(data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) == 0 &&
				data->index != NULL)
			{
				(void)git_index_remove(data->index, delta->old_file.path, 0);
			}
		}
	}

	git_vector_foreach(&data->removes, i, str) {
		error = git_futils_rmdir_r(str, fullpath->ptr, flg);
		if (error < 0)
			return error;

		data->completed_steps++;
		report_progress(data, str);

		if ((data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) == 0 &&
			data->index != NULL)
		{
			if (str[strlen(str) - 1] == '/')
				(void)git_index_remove_directory(data->index, str, 0);
			else
				(void)git_index_remove(data->index, str, 0);
		}
	}

	return 0;
}