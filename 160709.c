void test_checkout_nasty__dotcapitalgit_backslash_path(void)
{
#ifdef GIT_WIN32
	test_checkout_fails("refs/heads/dotcapitalgit_backslash_path", ".GIT/foobar");
#endif
}