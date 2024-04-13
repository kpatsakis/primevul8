static checkout_conflictdata *checkout_conflicts_search_branch(
	checkout_data *data,
	const char *path)
{
	checkout_conflictdata *conflict;
	size_t i;

	git_vector_foreach(&data->update_conflicts, i, conflict) {
		int cmp = -1;

		if (conflict->ancestor)
			break;

		if (conflict->ours)
			cmp = checkout_conflicts_cmp_entry(path, conflict->ours);
		else if (conflict->theirs)
			cmp = checkout_conflicts_cmp_entry(path, conflict->theirs);

		if (cmp == 0)
			return conflict;
	}

	return NULL;
}