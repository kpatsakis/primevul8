static int checkout_path_suffixed(git_buf *path, const char *suffix)
{
	size_t path_len;
	int i = 0, error = 0;

	if ((error = git_buf_putc(path, '~')) < 0 || (error = git_buf_puts(path, suffix)) < 0)
		return -1;

	path_len = git_buf_len(path);

	while (git_path_exists(git_buf_cstr(path)) && i < INT_MAX) {
		git_buf_truncate(path, path_len);

		if ((error = git_buf_putc(path, '_')) < 0 ||
			(error = git_buf_printf(path, "%d", i)) < 0)
			return error;

		i++;
	}

	if (i == INT_MAX) {
		git_buf_truncate(path, path_len);

		git_error_set(GIT_ERROR_CHECKOUT, "could not write '%s': working directory file exists", path->ptr);
		return GIT_EEXISTS;
	}

	return 0;
}