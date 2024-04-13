static int blob_content_to_link(
	checkout_data *data,
	struct stat *st,
	git_blob *blob,
	const char *path)
{
	git_buf linktarget = GIT_BUF_INIT;
	int error;

	if ((error = mkpath2file(data, path, data->opts.dir_mode)) < 0)
		return error;

	if ((error = git_blob__getbuf(&linktarget, blob)) < 0)
		return error;

	if (data->can_symlink) {
		if ((error = p_symlink(git_buf_cstr(&linktarget), path)) < 0)
			git_error_set(GIT_ERROR_OS, "could not create symlink %s", path);
	} else {
		error = git_futils_fake_symlink(git_buf_cstr(&linktarget), path);
	}

	if (!error) {
		data->perfdata.stat_calls++;

		if ((error = p_lstat(path, st)) < 0)
			git_error_set(GIT_ERROR_CHECKOUT, "could not stat symlink %s", path);

		st->st_mode = GIT_FILEMODE_LINK;
	}

	git_buf_dispose(&linktarget);

	return error;
}