void test_checkout_nasty__dotgit_path(void)
{
	test_checkout_fails("refs/heads/dotgit_path", ".git/foobar");
}