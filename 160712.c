GIT_INLINE(int) checkout_conflict_detect_binary(git_repository *repo, checkout_conflictdata *conflict)
{
	git_blob *ancestor_blob = NULL, *our_blob = NULL, *their_blob = NULL;
	int error = 0;

	if (conflict->submodule)
		return 0;

	if (conflict->ancestor) {
		if ((error = git_blob_lookup(&ancestor_blob, repo, &conflict->ancestor->id)) < 0)
			goto done;

		conflict->binary = git_blob_is_binary(ancestor_blob);
	}

	if (!conflict->binary && conflict->ours) {
		if ((error = git_blob_lookup(&our_blob, repo, &conflict->ours->id)) < 0)
			goto done;

		conflict->binary = git_blob_is_binary(our_blob);
	}

	if (!conflict->binary && conflict->theirs) {
		if ((error = git_blob_lookup(&their_blob, repo, &conflict->theirs->id)) < 0)
			goto done;

		conflict->binary = git_blob_is_binary(their_blob);
	}

done:
	git_blob_free(ancestor_blob);
	git_blob_free(our_blob);
	git_blob_free(their_blob);

	return error;
}