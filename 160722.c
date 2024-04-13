void test_checkout_nasty__honors_core_protectntfs(void)
{
	cl_repo_set_bool(repo, "core.protectNTFS", true);

	test_checkout_fails("refs/heads/dotgit_backslash_path", ".git/foobar");
	test_checkout_fails("refs/heads/dotcapitalgit_backslash_path", ".GIT/foobar");
	test_checkout_fails("refs/heads/dot_git_dot", ".git/foobar");
	test_checkout_fails("refs/heads/git_tilde1", ".git/foobar");
}