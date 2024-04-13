void test_checkout_nasty__dot_path(void)
{
	test_checkout_fails("refs/heads/dot_path", "./foobar");
}