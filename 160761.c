void test_checkout_nasty__dotdot_path(void)
{
	test_checkout_fails("refs/heads/dotdot_path", "foobar");
}