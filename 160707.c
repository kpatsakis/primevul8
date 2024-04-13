static int checkout_write_content(
	checkout_data *data,
	const git_oid *oid,
	const char *full_path,
	const char *hint_path,
	unsigned int mode,
	struct stat *st)
{
	int error = 0;
	git_blob *blob;

	if ((error = git_blob_lookup(&blob, data->repo, oid)) < 0)
		return error;

	if (S_ISLNK(mode))
		error = blob_content_to_link(data, st, blob, full_path);
	else
		error = blob_content_to_file(data, st, blob, full_path, hint_path, mode);

	git_blob_free(blob);

	/* if we try to create the blob and an existing directory blocks it from
	 * being written, then there must have been a typechange conflict in a
	 * parent directory - suppress the error and try to continue.
	 */
	if ((data->strategy & GIT_CHECKOUT_ALLOW_CONFLICTS) != 0 &&
		(error == GIT_ENOTFOUND || error == GIT_EEXISTS))
	{
		git_error_clear();
		error = 0;
	}

	return error;
}