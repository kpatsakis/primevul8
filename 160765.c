static int checkout_conflicts_load(checkout_data *data, git_iterator *workdir, git_vector *pathspec)
{
	git_index *index;

	/* Only write conficts from sources that have them: indexes. */
	if ((index = git_iterator_index(data->target)) == NULL)
		return 0;

	data->update_conflicts._cmp = checkout_conflictdata_cmp;

	if (checkout_conflicts_foreach(data, index, workdir, pathspec, checkout_conflict_append_update, data) < 0)
		return -1;

	/* Collect the REUC and NAME entries */
	data->update_reuc = &index->reuc;
	data->update_names = &index->names;

	return 0;
}