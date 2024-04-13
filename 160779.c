static int checkout_conflict_append_update(
	const git_index_entry *ancestor,
	const git_index_entry *ours,
	const git_index_entry *theirs,
	void *payload)
{
	checkout_data *data = payload;
	checkout_conflictdata *conflict;
	int error;

	conflict = git__calloc(1, sizeof(checkout_conflictdata));
	GIT_ERROR_CHECK_ALLOC(conflict);

	conflict->ancestor = ancestor;
	conflict->ours = ours;
	conflict->theirs = theirs;

	if ((error = checkout_conflict_detect_submodule(conflict)) < 0 ||
		(error = checkout_conflict_detect_binary(data->repo, conflict)) < 0)
	{
		git__free(conflict);
		return error;
	}

	if (git_vector_insert(&data->update_conflicts, conflict))
		return -1;

	return 0;
}