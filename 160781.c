void test_checkout_nasty__git_tilde1(void)
{
	test_checkout_fails("refs/heads/git_tilde1", ".git/foobar");
	test_checkout_fails("refs/heads/git_tilde1", "git~1/foobar");
}