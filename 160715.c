static void test_checkout_passes(const char *refname, const char *filename)
{
	git_oid commit_id;
	git_commit *commit;
	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	git_buf path = GIT_BUF_INIT;

	cl_git_pass(git_buf_joinpath(&path, repo_name, filename));

	cl_git_pass(git_reference_name_to_id(&commit_id, repo, refname));
	cl_git_pass(git_commit_lookup(&commit, repo, &commit_id));

	opts.checkout_strategy = GIT_CHECKOUT_FORCE |
		GIT_CHECKOUT_DONT_UPDATE_INDEX;

	cl_git_pass(git_checkout_tree(repo, (const git_object *)commit, &opts));
	cl_assert(!git_path_exists(path.ptr));

	git_commit_free(commit);
	git_buf_dispose(&path);
}