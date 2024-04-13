static int checkout_write_merge(
	checkout_data *data,
	checkout_conflictdata *conflict)
{
	git_buf our_label = GIT_BUF_INIT, their_label = GIT_BUF_INIT,
		path_suffixed = GIT_BUF_INIT, path_workdir = GIT_BUF_INIT,
		in_data = GIT_BUF_INIT, out_data = GIT_BUF_INIT;
	git_merge_file_options opts = GIT_MERGE_FILE_OPTIONS_INIT;
	git_merge_file_result result = {0};
	git_filebuf output = GIT_FILEBUF_INIT;
	git_filter_list *fl = NULL;
	git_filter_options filter_opts = GIT_FILTER_OPTIONS_INIT;
	int error = 0;

	if (data->opts.checkout_strategy & GIT_CHECKOUT_CONFLICT_STYLE_DIFF3)
		opts.flags |= GIT_MERGE_FILE_STYLE_DIFF3;

	opts.ancestor_label = data->opts.ancestor_label ?
		data->opts.ancestor_label : "ancestor";
	opts.our_label = data->opts.our_label ?
		data->opts.our_label : "ours";
	opts.their_label = data->opts.their_label ?
		data->opts.their_label : "theirs";

	/* If all the paths are identical, decorate the diff3 file with the branch
	 * names.  Otherwise, append branch_name:path.
	 */
	if (conflict->ours && conflict->theirs &&
		strcmp(conflict->ours->path, conflict->theirs->path) != 0) {

		if ((error = conflict_entry_name(
			&our_label, opts.our_label, conflict->ours->path)) < 0 ||
			(error = conflict_entry_name(
			&their_label, opts.their_label, conflict->theirs->path)) < 0)
			goto done;

		opts.our_label = git_buf_cstr(&our_label);
		opts.their_label = git_buf_cstr(&their_label);
	}

	if ((error = git_merge_file_from_index(&result, data->repo,
		conflict->ancestor, conflict->ours, conflict->theirs, &opts)) < 0)
		goto done;

	if (result.path == NULL || result.mode == 0) {
		git_error_set(GIT_ERROR_CHECKOUT, "could not merge contents of file");
		error = GIT_ECONFLICT;
		goto done;
	}

	if ((error = checkout_merge_path(&path_workdir, data, conflict, &result)) < 0)
		goto done;

	if ((data->strategy & GIT_CHECKOUT_UPDATE_ONLY) != 0 &&
		(error = checkout_safe_for_update_only(data, git_buf_cstr(&path_workdir), result.mode)) <= 0)
		goto done;

	if (!data->opts.disable_filters) {
		in_data.ptr = (char *)result.ptr;
		in_data.size = result.len;

		filter_opts.attr_session = &data->attr_session;
		filter_opts.temp_buf = &data->tmp;

		if ((error = git_filter_list__load_ext(
				&fl, data->repo, NULL, git_buf_cstr(&path_workdir),
				GIT_FILTER_TO_WORKTREE, &filter_opts)) < 0 ||
			(error = git_filter_list_apply_to_data(&out_data, fl, &in_data)) < 0)
			goto done;
	} else {
		out_data.ptr = (char *)result.ptr;
		out_data.size = result.len;
	}

	if ((error = mkpath2file(data, path_workdir.ptr, data->opts.dir_mode)) < 0 ||
		(error = git_filebuf_open(&output, git_buf_cstr(&path_workdir), GIT_FILEBUF_DO_NOT_BUFFER, result.mode)) < 0 ||
		(error = git_filebuf_write(&output, out_data.ptr, out_data.size)) < 0 ||
		(error = git_filebuf_commit(&output)) < 0)
		goto done;

done:
	git_filter_list_free(fl);

	git_buf_dispose(&out_data);
	git_buf_dispose(&our_label);
	git_buf_dispose(&their_label);

	git_merge_file_result_free(&result);
	git_buf_dispose(&path_workdir);
	git_buf_dispose(&path_suffixed);

	return error;
}