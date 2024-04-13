static int checkout_blob(
	checkout_data *data,
	const git_diff_file *file)
{
	git_buf *fullpath;
	struct stat st;
	int error = 0;

	if (checkout_target_fullpath(&fullpath, data, file->path) < 0)
		return -1;

	if ((data->strategy & GIT_CHECKOUT_UPDATE_ONLY) != 0) {
		int rval = checkout_safe_for_update_only(
			data, fullpath->ptr, file->mode);

		if (rval <= 0)
			return rval;
	}

	error = checkout_write_content(
		data, &file->id, fullpath->ptr, NULL, file->mode, &st);

	/* update the index unless prevented */
	if (!error && (data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) == 0)
		error = checkout_update_index(data, file, &st);

	/* update the submodule data if this was a new .gitmodules file */
	if (!error && strcmp(file->path, ".gitmodules") == 0)
		data->reload_submodules = true;

	return error;
}