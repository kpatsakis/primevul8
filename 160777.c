static int checkout_target_fullpath(
	git_buf **out, checkout_data *data, const char *path)
{
	git_buf_truncate(&data->target_path, data->target_len);

	if (path && git_buf_puts(&data->target_path, path) < 0)
		return -1;

	*out = &data->target_path;

	return 0;
}