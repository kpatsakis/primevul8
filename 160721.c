void test_checkout_nasty__symlink1(void)
{
	test_checkout_passes("refs/heads/symlink1", ".git/foobar");
}