void test_checkout_nasty__dotcapitalgit_path(void)
{
	test_checkout_fails("refs/heads/dotcapitalgit_path", ".GIT/foobar");
}