static bool checkout_is_workdir_modified(
	checkout_data *data,
	const git_diff_file *baseitem,
	const git_diff_file *newitem,
	const git_index_entry *wditem)
{
	git_oid oid;
	const git_index_entry *ie;

	/* handle "modified" submodule */
	if (wditem->mode == GIT_FILEMODE_COMMIT) {
		git_submodule *sm;
		unsigned int sm_status = 0;
		const git_oid *sm_oid = NULL;
		bool rval = false;

		if (git_submodule_lookup(&sm, data->repo, wditem->path) < 0) {
			git_error_clear();
			return true;
		}

		if (git_submodule_status(&sm_status, data->repo, wditem->path, GIT_SUBMODULE_IGNORE_UNSPECIFIED) < 0 ||
			GIT_SUBMODULE_STATUS_IS_WD_DIRTY(sm_status))
			rval = true;
		else if ((sm_oid = git_submodule_wd_id(sm)) == NULL)
			rval = false;
		else
			rval = (git_oid__cmp(&baseitem->id, sm_oid) != 0);

		git_submodule_free(sm);
		return rval;
	}

	/*
	 * Look at the cache to decide if the workdir is modified: if the
	 * cache contents match the workdir contents, then we do not need
	 * to examine the working directory directly, instead we can
	 * examine the cache to see if _it_ has been modified.  This allows
	 * us to avoid touching the disk.
	 */
	ie = git_index_get_bypath(data->index, wditem->path, 0);

	if (ie != NULL &&
		git_index_time_eq(&wditem->mtime, &ie->mtime) &&
		wditem->file_size == ie->file_size &&
		!is_filemode_changed(wditem->mode, ie->mode, data->respect_filemode)) {

		/* The workdir is modified iff the index entry is modified */
		return !is_workdir_base_or_new(&ie->id, baseitem, newitem) ||
			is_filemode_changed(baseitem->mode, ie->mode, data->respect_filemode);
	}

	/* depending on where base is coming from, we may or may not know
	 * the actual size of the data, so we can't rely on this shortcut.
	 */
	if (baseitem->size && wditem->file_size != baseitem->size)
		return true;

	/* if the workdir item is a directory, it cannot be a modified file */
	if (S_ISDIR(wditem->mode))
		return false;

	if (is_filemode_changed(baseitem->mode, wditem->mode, data->respect_filemode))
		return true;

	if (git_diff__oid_for_entry(&oid, data->diff, wditem, wditem->mode, NULL) < 0)
		return false;

	/* Allow the checkout if the workdir is not modified *or* if the checkout
	 * target's contents are already in the working directory.
	 */
	return !is_workdir_base_or_new(&oid, baseitem, newitem);
}