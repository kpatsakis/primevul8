void test_checkout_nasty__dotdot_tree(void)
{
	test_checkout_fails("refs/heads/dotdot_tree", "foobar");
}