void test_checkout_nasty__dotgit_backslash_path(void)
{
#ifdef GIT_WIN32
	test_checkout_fails("refs/heads/dotgit_backslash_path", ".git/foobar");
#endif
}