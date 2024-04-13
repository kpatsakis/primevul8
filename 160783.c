void test_checkout_nasty__honors_core_protecthfs(void)
{
	cl_repo_set_bool(repo, "core.protectHFS", true);

	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_1", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_2", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_3", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_4", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_5", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_6", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_7", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_8", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_9", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_10", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_11", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_12", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_13", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_14", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_15", ".git/foobar");
	test_checkout_fails("refs/heads/dotgit_hfs_ignorable_16", ".git/foobar");
}