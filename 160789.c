static int checkout_conflict_add(
	checkout_data *data,
	const git_index_entry *conflict)
{
	int error = git_index_remove(data->index, conflict->path, 0);

	if (error == GIT_ENOTFOUND)
		git_error_clear();
	else if (error < 0)
		return error;

	return git_index_add(data->index, conflict);
}