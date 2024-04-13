void test_checkout_nasty__git_custom_shortname(void)
{
#ifdef GIT_WIN32
	if (!cl_sandbox_supports_8dot3())
		clar__skip();

	cl_must_pass(p_rename("nasty/.git", "nasty/_temp"));
	cl_git_write2file("nasty/git~1", "", 0, O_RDWR|O_CREAT, 0666);
	cl_must_pass(p_rename("nasty/_temp", "nasty/.git"));
	test_checkout_fails("refs/heads/git_tilde2", ".git/foobar");
#endif
}