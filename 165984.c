static int commit_error(git_commit_list_node *commit, const char *msg)
{
	char commit_oid[GIT_OID_HEXSZ + 1];
	git_oid_fmt(commit_oid, &commit->oid);
	commit_oid[GIT_OID_HEXSZ] = '\0';

	git_error_set(GIT_ERROR_ODB, "failed to parse commit %s - %s", commit_oid, msg);

	return -1;
}