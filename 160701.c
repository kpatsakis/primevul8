static bool wd_item_is_removable(
	checkout_data *data, const git_index_entry *wd)
{
	git_buf *full;

	if (wd->mode != GIT_FILEMODE_TREE)
		return true;

	if (checkout_target_fullpath(&full, data, wd->path) < 0)
		return false;

	return !full || !git_path_contains(full, DOT_GIT);
}