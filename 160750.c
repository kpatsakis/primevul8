void test_checkout_nasty__initialize(void)
{
	repo = cl_git_sandbox_init(repo_name);

	GIT_INIT_STRUCTURE(&checkout_opts, GIT_CHECKOUT_OPTIONS_VERSION);
	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
}