static int checkout_get_update_conflicts(
	checkout_data *data,
	git_iterator *workdir,
	git_vector *pathspec)
{
	int error = 0;

	if (data->strategy & GIT_CHECKOUT_SKIP_UNMERGED)
		return 0;

	if ((error = checkout_conflicts_load(data, workdir, pathspec)) < 0 ||
		(error = checkout_conflicts_coalesce_renames(data)) < 0 ||
		(error = checkout_conflicts_mark_directoryfile(data)) < 0)
		goto done;

done:
	return error;
}