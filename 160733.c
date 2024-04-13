void test_checkout_nasty__dot_path_two(void)
{
	test_checkout_fails("refs/heads/dot_path_two", "foo/.");
}