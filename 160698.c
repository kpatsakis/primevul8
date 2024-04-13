void test_checkout_nasty__dotgit_tree(void)
{
	test_checkout_fails("refs/heads/dotgit_tree", ".git/foobar");
}