int git_checkout_options_init(git_checkout_options *opts, unsigned int version)
{
	GIT_INIT_STRUCTURE_FROM_TEMPLATE(
		opts, version, git_checkout_options, GIT_CHECKOUT_OPTIONS_INIT);
	return 0;
}