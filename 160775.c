static bool checkout_is_empty_dir(checkout_data *data, const char *path)
{
	git_buf *fullpath;

	if (checkout_target_fullpath(&fullpath, data, path) < 0)
		return false;

	return git_path_is_empty_dir(fullpath->ptr);
}