void test_checkout_nasty__dotdot_dotcapitalgit_path(void)
{
	test_checkout_fails("refs/heads/dotdot_dotcapitalgit_path", ".GIT/foobar");
}