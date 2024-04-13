void test_checkout_nasty__dot_git_colon_stuff(void)
{
#ifdef GIT_WIN32
	test_checkout_fails("refs/heads/dot_git_colon_stuff", ".git/foobar");
#endif
}