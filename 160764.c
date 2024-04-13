void test_checkout_nasty__dotcapitalgit_tree(void)
{
	test_checkout_fails("refs/heads/dotcapitalgit_tree", ".GIT/foobar");
}