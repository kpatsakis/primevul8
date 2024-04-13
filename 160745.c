static int checkout_deferred_remove(git_repository *repo, const char *path)
{
#if 0
	int error = git_futils_rmdir_r(
		path, data->opts.target_directory, GIT_RMDIR_EMPTY_PARENTS);

	if (error == GIT_ENOTFOUND) {
		error = 0;
		git_error_clear();
	}

	return error;
#else
	GIT_UNUSED(repo);
	GIT_UNUSED(path);
	assert(false);
	return 0;
#endif
}