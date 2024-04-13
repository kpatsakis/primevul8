static int checkout_submodule(
	checkout_data *data,
	const git_diff_file *file)
{
	bool remove_existing = should_remove_existing(data);
	int error = 0;

	/* Until submodules are supported, UPDATE_ONLY means do nothing here */
	if ((data->strategy & GIT_CHECKOUT_UPDATE_ONLY) != 0)
		return 0;

	if ((error = checkout_mkdir(
			data,
			file->path, data->opts.target_directory, data->opts.dir_mode,
			remove_existing ? MKDIR_REMOVE_EXISTING : MKDIR_NORMAL)) < 0)
		return error;

	if ((error = git_submodule_lookup(NULL, data->repo, file->path)) < 0) {
		/* I've observed repos with submodules in the tree that do not
		 * have a .gitmodules - core Git just makes an empty directory
		 */
		if (error == GIT_ENOTFOUND) {
			git_error_clear();
			return checkout_submodule_update_index(data, file);
		}

		return error;
	}

	/* TODO: Support checkout_strategy options.  Two circumstances:
	 * 1 - submodule already checked out, but we need to move the HEAD
	 *     to the new OID, or
	 * 2 - submodule not checked out and we should recursively check it out
	 *
	 * Checkout will not execute a pull on the submodule, but a clone
	 * command should probably be able to.  Do we need a submodule callback?
	 */

	return checkout_submodule_update_index(data, file);
}