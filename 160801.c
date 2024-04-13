GIT_INLINE(bool) conflict_pathspec_match(
	checkout_data *data,
	git_iterator *workdir,
	git_vector *pathspec,
	const git_index_entry *ancestor,
	const git_index_entry *ours,
	const git_index_entry *theirs)
{
	/* if the pathspec matches ours *or* theirs, proceed */
	if (ours && git_pathspec__match(pathspec, ours->path,
		(data->strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH) != 0,
		git_iterator_ignore_case(workdir), NULL, NULL))
		return true;

	if (theirs && git_pathspec__match(pathspec, theirs->path,
		(data->strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH) != 0,
		git_iterator_ignore_case(workdir), NULL, NULL))
		return true;

	if (ancestor && git_pathspec__match(pathspec, ancestor->path,
		(data->strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH) != 0,
		git_iterator_ignore_case(workdir), NULL, NULL))
		return true;

	return false;
}