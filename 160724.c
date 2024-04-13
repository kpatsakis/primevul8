void test_checkout_nasty__dot_dotgit_tree(void)
{
	test_checkout_fails("refs/heads/dot_dotgit_tree", ".git/foobar");
}