static int checkout_conflict_append_remove(
	const git_index_entry *ancestor,
	const git_index_entry *ours,
	const git_index_entry *theirs,
	void *payload)
{
	checkout_data *data = payload;
	const char *name;

	assert(ancestor || ours || theirs);

	if (ancestor)
		name = git__strdup(ancestor->path);
	else if (ours)
		name = git__strdup(ours->path);
	else if (theirs)
		name = git__strdup(theirs->path);
	else
		abort();

	GIT_ERROR_CHECK_ALLOC(name);

	return git_vector_insert(&data->remove_conflicts, (char *)name);
}