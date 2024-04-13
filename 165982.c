int git_commit_list_time_cmp(const void *a, const void *b)
{
	int64_t time_a = ((git_commit_list_node *) a)->time;
	int64_t time_b = ((git_commit_list_node *) b)->time;

	if (time_a < time_b)
		return 1;
	if (time_a > time_b)
		return -1;

	return 0;
}