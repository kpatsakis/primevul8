void test_checkout_nasty__dotdot_dotgit_path(void)
{
	test_checkout_fails("refs/heads/dotdot_dotgit_path", ".git/foobar");
}