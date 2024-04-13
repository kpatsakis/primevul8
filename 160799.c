static int checkout_data_init(
	checkout_data *data,
	git_iterator *target,
	const git_checkout_options *proposed)
{
	int error = 0;
	git_repository *repo = git_iterator_owner(target);

	memset(data, 0, sizeof(*data));

	if (!repo) {
		git_error_set(GIT_ERROR_CHECKOUT, "cannot checkout nothing");
		return -1;
	}

	if ((!proposed || !proposed->target_directory) &&
		(error = git_repository__ensure_not_bare(repo, "checkout")) < 0)
		return error;

	data->repo = repo;
	data->target = target;

	GIT_ERROR_CHECK_VERSION(
		proposed, GIT_CHECKOUT_OPTIONS_VERSION, "git_checkout_options");

	if (!proposed)
		GIT_INIT_STRUCTURE(&data->opts, GIT_CHECKOUT_OPTIONS_VERSION);
	else
		memmove(&data->opts, proposed, sizeof(git_checkout_options));

	if (!data->opts.target_directory)
		data->opts.target_directory = git_repository_workdir(repo);
	else if (!git_path_isdir(data->opts.target_directory) &&
			 (error = checkout_mkdir(data,
				data->opts.target_directory, NULL,
				GIT_DIR_MODE, GIT_MKDIR_VERIFY_DIR)) < 0)
		goto cleanup;

	if ((error = git_repository_index(&data->index, data->repo)) < 0)
		goto cleanup;

	/* refresh config and index content unless NO_REFRESH is given */
	if ((data->opts.checkout_strategy & GIT_CHECKOUT_NO_REFRESH) == 0) {
		git_config *cfg;

		if ((error = git_repository_config__weakptr(&cfg, repo)) < 0)
			goto cleanup;

		/* Reload the repository index (unless we're checking out the
		 * index; then it has the changes we're trying to check out
		 * and those should not be overwritten.)
		 */
		if (data->index != git_iterator_index(target)) {
			if (data->opts.checkout_strategy & GIT_CHECKOUT_FORCE) {
				/* When forcing, we can blindly re-read the index */
				if ((error = git_index_read(data->index, false)) < 0)
					goto cleanup;
			} else {
				/*
				 * When not being forced, we need to check for unresolved
				 * conflicts and unsaved changes in the index before
				 * proceeding.
				 */
				if (git_index_has_conflicts(data->index)) {
					error = GIT_ECONFLICT;
					git_error_set(GIT_ERROR_CHECKOUT,
						"unresolved conflicts exist in the index");
					goto cleanup;
				}

				if ((error = git_index_read_safely(data->index)) < 0)
					goto cleanup;
			}

			/* clean conflict data in the current index */
			git_index_name_clear(data->index);
			git_index_reuc_clear(data->index);
		}
	}

	/* if you are forcing, allow all safe updates, plus recreate missing */
	if ((data->opts.checkout_strategy & GIT_CHECKOUT_FORCE) != 0)
		data->opts.checkout_strategy |= GIT_CHECKOUT_SAFE |
			GIT_CHECKOUT_RECREATE_MISSING;

	/* if the repository does not actually have an index file, then this
	 * is an initial checkout (perhaps from clone), so we allow safe updates
	 */
	if (!data->index->on_disk &&
		(data->opts.checkout_strategy & GIT_CHECKOUT_SAFE) != 0)
		data->opts.checkout_strategy |= GIT_CHECKOUT_RECREATE_MISSING;

	data->strategy = data->opts.checkout_strategy;

	/* opts->disable_filters is false by default */

	if (!data->opts.dir_mode)
		data->opts.dir_mode = GIT_DIR_MODE;

	if (!data->opts.file_open_flags)
		data->opts.file_open_flags = O_CREAT | O_TRUNC | O_WRONLY;

	data->pfx = git_pathspec_prefix(&data->opts.paths);

	if ((error = git_repository__configmap_lookup(
			 &data->can_symlink, repo, GIT_CONFIGMAP_SYMLINKS)) < 0)
		goto cleanup;

	if ((error = git_repository__configmap_lookup(
			 &data->respect_filemode, repo, GIT_CONFIGMAP_FILEMODE)) < 0)
		goto cleanup;

	if (!data->opts.baseline && !data->opts.baseline_index) {
		data->opts_free_baseline = true;
		error = 0;

		/* if we don't have an index, this is an initial checkout and
		 * should be against an empty baseline
		 */
		if (data->index->on_disk)
			error = checkout_lookup_head_tree(&data->opts.baseline, repo);

		if (error == GIT_EUNBORNBRANCH) {
			error = 0;
			git_error_clear();
		}

		if (error < 0)
			goto cleanup;
	}

	if ((data->opts.checkout_strategy &
		(GIT_CHECKOUT_CONFLICT_STYLE_MERGE | GIT_CHECKOUT_CONFLICT_STYLE_DIFF3)) == 0) {
		git_config_entry *conflict_style = NULL;
		git_config *cfg = NULL;

		if ((error = git_repository_config__weakptr(&cfg, repo)) < 0 ||
			(error = git_config_get_entry(&conflict_style, cfg, "merge.conflictstyle")) < 0 ||
			error == GIT_ENOTFOUND)
			;
		else if (error)
			goto cleanup;
		else if (strcmp(conflict_style->value, "merge") == 0)
			data->opts.checkout_strategy |= GIT_CHECKOUT_CONFLICT_STYLE_MERGE;
		else if (strcmp(conflict_style->value, "diff3") == 0)
			data->opts.checkout_strategy |= GIT_CHECKOUT_CONFLICT_STYLE_DIFF3;
		else {
			git_error_set(GIT_ERROR_CHECKOUT, "unknown style '%s' given for 'merge.conflictstyle'",
				conflict_style->value);
			error = -1;
			git_config_entry_free(conflict_style);
			goto cleanup;
		}
		git_config_entry_free(conflict_style);
	}

	git_pool_init(&data->pool, 1);

	if ((error = git_vector_init(&data->removes, 0, git__strcmp_cb)) < 0 ||
	    (error = git_vector_init(&data->remove_conflicts, 0, NULL)) < 0 ||
	    (error = git_vector_init(&data->update_conflicts, 0, NULL)) < 0 ||
	    (error = git_buf_puts(&data->target_path, data->opts.target_directory)) < 0 ||
	    (error = git_path_to_dir(&data->target_path)) < 0 ||
	    (error = git_strmap_new(&data->mkdir_map)) < 0)
		goto cleanup;

	data->target_len = git_buf_len(&data->target_path);

	git_attr_session__init(&data->attr_session, data->repo);

cleanup:
	if (error < 0)
		checkout_data_clear(data);

	return error;
}