static checkout_conflictdata *checkout_conflicts_search_ancestor(
	checkout_data *data,
	const char *path)
{
	size_t pos;

	if (git_vector_bsearch2(&pos, &data->update_conflicts, checkout_conflicts_cmp_ancestor, path) < 0)
		return NULL;

	return git_vector_get(&data->update_conflicts, pos);
}