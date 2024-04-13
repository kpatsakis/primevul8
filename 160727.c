GIT_INLINE(int) checkout_conflict_detect_submodule(checkout_conflictdata *conflict)
{
	conflict->submodule = ((conflict->ancestor && S_ISGITLINK(conflict->ancestor->mode)) ||
		(conflict->ours && S_ISGITLINK(conflict->ours->mode)) ||
		(conflict->theirs && S_ISGITLINK(conflict->theirs->mode)));
	return 0;
}