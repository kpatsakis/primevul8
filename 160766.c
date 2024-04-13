void test_checkout_nasty__dotdot_dotgit_tree(void)
{
	test_checkout_fails("refs/heads/dotdot_dotgit_tree", ".git/foobar");
}