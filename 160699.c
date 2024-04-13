void test_checkout_nasty__symlink2(void)
{
	test_checkout_passes("refs/heads/symlink2", ".git/foobar");
}