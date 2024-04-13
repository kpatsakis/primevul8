void test_checkout_nasty__symlink3(void)
{
	test_checkout_passes("refs/heads/symlink3", ".git/foobar");
}