void test_checkout_nasty__gitmodules_symlink(void)
{
	cl_repo_set_bool(repo, "core.protectHFS", true);
	test_checkout_fails("refs/heads/gitmodules-symlink", ".gitmodules");
	cl_repo_set_bool(repo, "core.protectHFS", false);

	cl_repo_set_bool(repo, "core.protectNTFS", true);
	test_checkout_fails("refs/heads/gitmodules-symlink", ".gitmodules");
	cl_repo_set_bool(repo, "core.protectNTFS", false);

	test_checkout_fails("refs/heads/gitmodules-symlink", ".gitmodules");
}