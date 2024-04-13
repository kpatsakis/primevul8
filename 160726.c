void test_checkout_nasty__dot_tree(void)
{
	test_checkout_fails("refs/heads/dot_tree", "foobar");
}