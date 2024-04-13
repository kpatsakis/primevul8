void test_checkout_nasty__dot_dotgit_path(void)
{
	test_checkout_fails("refs/heads/dot_dotgit_path", ".git/foobar");
}