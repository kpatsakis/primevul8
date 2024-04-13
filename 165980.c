git_commit_list_node *git_commit_list_alloc_node(git_revwalk *walk)
{
	return (git_commit_list_node *)git_pool_mallocz(&walk->commit_pool, 1);
}