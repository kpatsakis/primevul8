int git_checkout_head(
	git_repository *repo,
	const git_checkout_options *opts)
{
	assert(repo);
	return git_checkout_tree(repo, NULL, opts);
}