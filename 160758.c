static int checkout_conflicts_mark_directoryfile(
	checkout_data *data)
{
	git_index *index;
	checkout_conflictdata *conflict;
	const git_index_entry *entry;
	size_t i, j, len;
	const char *path;
	int prefixed, error = 0;

	if ((index = git_iterator_index(data->target)) == NULL)
		return 0;

	len = git_index_entrycount(index);

	/* Find d/f conflicts */
	git_vector_foreach(&data->update_conflicts, i, conflict) {
		if ((conflict->ours && conflict->theirs) ||
			(!conflict->ours && !conflict->theirs))
			continue;

		path = conflict->ours ?
			conflict->ours->path : conflict->theirs->path;

		if ((error = git_index_find(&j, index, path)) < 0) {
			if (error == GIT_ENOTFOUND)
				git_error_set(GIT_ERROR_INDEX,
					"index inconsistency, could not find entry for expected conflict '%s'", path);

			goto done;
		}

		for (; j < len; j++) {
			if ((entry = git_index_get_byindex(index, j)) == NULL) {
				git_error_set(GIT_ERROR_INDEX,
					"index inconsistency, truncated index while loading expected conflict '%s'", path);
				error = -1;
				goto done;
			}

			prefixed = git_path_equal_or_prefixed(path, entry->path, NULL);

			if (prefixed == GIT_PATH_EQUAL)
				continue;

			if (prefixed == GIT_PATH_PREFIX)
				conflict->directoryfile = 1;

			break;
		}
	}

done:
	return error;
}