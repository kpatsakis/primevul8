void test_checkout_nasty__dot_git_dot(void)
{
#ifdef GIT_WIN32
	test_checkout_fails("refs/heads/dot_git_dot", ".git/foobar");
#endif
}