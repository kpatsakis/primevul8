void test_checkout_nasty__dot_backslash_dotcapitalgit_path(void)
{
#ifdef GIT_WIN32
	test_checkout_fails("refs/heads/dot_backslash_dotcapitalgit_path", ".GIT/foobar");
#endif
}