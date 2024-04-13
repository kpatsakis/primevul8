static int blob_content_to_file(
	checkout_data *data,
	struct stat *st,
	git_blob *blob,
	const char *path,
	const char *hint_path,
	mode_t entry_filemode)
{
	int flags = data->opts.file_open_flags;
	mode_t file_mode = data->opts.file_mode ?
		data->opts.file_mode : entry_filemode;
	git_filter_options filter_opts = GIT_FILTER_OPTIONS_INIT;
	struct checkout_stream writer;
	mode_t mode;
	git_filter_list *fl = NULL;
	int fd;
	int error = 0;

	if (hint_path == NULL)
		hint_path = path;

	if ((error = mkpath2file(data, path, data->opts.dir_mode)) < 0)
		return error;

	if (flags <= 0)
		flags = O_CREAT | O_TRUNC | O_WRONLY;
	if (!(mode = file_mode))
		mode = GIT_FILEMODE_BLOB;

	if ((fd = p_open(path, flags, mode)) < 0) {
		git_error_set(GIT_ERROR_OS, "could not open '%s' for writing", path);
		return fd;
	}

	filter_opts.attr_session = &data->attr_session;
	filter_opts.temp_buf = &data->tmp;

	if (!data->opts.disable_filters &&
		(error = git_filter_list__load_ext(
			&fl, data->repo, blob, hint_path,
			GIT_FILTER_TO_WORKTREE, &filter_opts))) {
		p_close(fd);
		return error;
	}

	/* setup the writer */
	memset(&writer, 0, sizeof(struct checkout_stream));
	writer.base.write = checkout_stream_write;
	writer.base.close = checkout_stream_close;
	writer.base.free = checkout_stream_free;
	writer.path = path;
	writer.fd = fd;
	writer.open = 1;

	error = git_filter_list_stream_blob(fl, blob, &writer.base);

	assert(writer.open == 0);

	git_filter_list_free(fl);

	if (error < 0)
		return error;

	if (st) {
		data->perfdata.stat_calls++;

		if ((error = p_stat(path, st)) < 0) {
			git_error_set(GIT_ERROR_OS, "failed to stat '%s'", path);
			return error;
		}

		st->st_mode = entry_filemode;
	}

	return 0;
}