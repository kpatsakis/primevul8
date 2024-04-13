void test_checkout_nasty__dot_dotcapitalgit_path(void)
{
	test_checkout_fails("refs/heads/dot_dotcapitalgit_path", ".GIT/foobar");
}