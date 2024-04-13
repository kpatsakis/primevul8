static int checkout_get_remove_conflicts(
	checkout_data *data,
	git_iterator *workdir,
	git_vector *pathspec)
{
	if ((data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) != 0)
		return 0;

	return checkout_conflicts_foreach(data, data->index, workdir, pathspec, checkout_conflict_append_remove, data);
}