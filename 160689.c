int checkout_conflictdata_empty(
	const git_vector *conflicts, size_t idx, void *payload)
{
	checkout_conflictdata *conflict;

	GIT_UNUSED(payload);

	if ((conflict = git_vector_get(conflicts, idx)) == NULL)
		return -1;

	if (conflict->ancestor || conflict->ours || conflict->theirs)
		return 0;

	git__free(conflict);
	return 1;
}