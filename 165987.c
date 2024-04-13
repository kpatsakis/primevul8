static int commit_quick_parse(
	git_revwalk *walk,
	git_commit_list_node *commit,
	const uint8_t *buffer,
	size_t buffer_len)
{
	const size_t parent_len = strlen("parent ") + GIT_OID_HEXSZ + 1;
	const uint8_t *buffer_end = buffer + buffer_len;
	const uint8_t *parents_start, *committer_start;
	int i, parents = 0;
	int64_t commit_time;

	buffer += strlen("tree ") + GIT_OID_HEXSZ + 1;

	parents_start = buffer;
	while (buffer + parent_len < buffer_end && memcmp(buffer, "parent ", strlen("parent ")) == 0) {
		parents++;
		buffer += parent_len;
	}

	commit->parents = alloc_parents(walk, commit, parents);
	GIT_ERROR_CHECK_ALLOC(commit->parents);

	buffer = parents_start;
	for (i = 0; i < parents; ++i) {
		git_oid oid;

		if (git_oid_fromstr(&oid, (const char *)buffer + strlen("parent ")) < 0)
			return -1;

		commit->parents[i] = git_revwalk__commit_lookup(walk, &oid);
		if (commit->parents[i] == NULL)
			return -1;

		buffer += parent_len;
	}

	commit->out_degree = (unsigned short)parents;

	if ((committer_start = buffer = memchr(buffer, '\n', buffer_end - buffer)) == NULL)
		return commit_error(commit, "object is corrupted");

	buffer++;

	if ((buffer = memchr(buffer, '\n', buffer_end - buffer)) == NULL)
		return commit_error(commit, "object is corrupted");

	/* Skip trailing spaces */
	while (buffer > committer_start && git__isspace(*buffer))
		buffer--;

	/* Seek for the beginning of the pack of digits */
	while (buffer > committer_start && git__isdigit(*buffer))
		buffer--;

	/* Skip potential timezone offset */
	if ((buffer > committer_start) && (*buffer == '+' || *buffer == '-')) {
		buffer--;

		while (buffer > committer_start && git__isspace(*buffer))
			buffer--;

		while (buffer > committer_start && git__isdigit(*buffer))
			buffer--;
	}

	if ((buffer == committer_start) ||
	    (git__strntol64(&commit_time, (char *)(buffer + 1),
			    buffer_end - buffer + 1, NULL, 10) < 0))
		return commit_error(commit, "cannot parse commit time");

	commit->time = commit_time;
	commit->parsed = 1;
	return 0;
}