int git_checkout_iterator(
	git_iterator *target,
	git_index *index,
	const git_checkout_options *opts)
{
	int error = 0;
	git_iterator *baseline = NULL, *workdir = NULL;
	git_iterator_options baseline_opts = GIT_ITERATOR_OPTIONS_INIT,
		workdir_opts = GIT_ITERATOR_OPTIONS_INIT;
	checkout_data data = {0};
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	uint32_t *actions = NULL;
	size_t *counts = NULL;

	/* initialize structures and options */
	error = checkout_data_init(&data, target, opts);
	if (error < 0)
		return error;

	diff_opts.flags =
		GIT_DIFF_INCLUDE_UNMODIFIED |
		GIT_DIFF_INCLUDE_UNREADABLE |
		GIT_DIFF_INCLUDE_UNTRACKED |
		GIT_DIFF_RECURSE_UNTRACKED_DIRS | /* needed to match baseline */
		GIT_DIFF_INCLUDE_IGNORED |
		GIT_DIFF_INCLUDE_TYPECHANGE |
		GIT_DIFF_INCLUDE_TYPECHANGE_TREES |
		GIT_DIFF_SKIP_BINARY_CHECK |
		GIT_DIFF_INCLUDE_CASECHANGE;
	if (data.opts.checkout_strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH)
		diff_opts.flags |= GIT_DIFF_DISABLE_PATHSPEC_MATCH;
	if (data.opts.paths.count > 0)
		diff_opts.pathspec = data.opts.paths;

	/* set up iterators */

	workdir_opts.flags = git_iterator_ignore_case(target) ?
		GIT_ITERATOR_IGNORE_CASE : GIT_ITERATOR_DONT_IGNORE_CASE;
	workdir_opts.flags |= GIT_ITERATOR_DONT_AUTOEXPAND;
	workdir_opts.start = data.pfx;
	workdir_opts.end = data.pfx;

	if ((error = git_iterator_reset_range(target, data.pfx, data.pfx)) < 0 ||
		(error = git_iterator_for_workdir_ext(
			&workdir, data.repo, data.opts.target_directory, index, NULL,
			&workdir_opts)) < 0)
		goto cleanup;

	baseline_opts.flags = git_iterator_ignore_case(target) ?
		GIT_ITERATOR_IGNORE_CASE : GIT_ITERATOR_DONT_IGNORE_CASE;
	baseline_opts.start = data.pfx;
	baseline_opts.end = data.pfx;
	if (opts && (opts->checkout_strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH)) {
		baseline_opts.pathlist.count = opts->paths.count;
		baseline_opts.pathlist.strings = opts->paths.strings;
	}

	if (data.opts.baseline_index) {
		if ((error = git_iterator_for_index(
				&baseline, git_index_owner(data.opts.baseline_index),
				data.opts.baseline_index, &baseline_opts)) < 0)
			goto cleanup;
	} else {
		if ((error = git_iterator_for_tree(
				&baseline, data.opts.baseline, &baseline_opts)) < 0)
			goto cleanup;
	}

	/* Should not have case insensitivity mismatch */
	assert(git_iterator_ignore_case(workdir) == git_iterator_ignore_case(baseline));

	/* Generate baseline-to-target diff which will include an entry for
	 * every possible update that might need to be made.
	 */
	if ((error = git_diff__from_iterators(
			&data.diff, data.repo, baseline, target, &diff_opts)) < 0)
		goto cleanup;

	/* Loop through diff (and working directory iterator) building a list of
	 * actions to be taken, plus look for conflicts and send notifications,
	 * then loop through conflicts.
	 */
	if ((error = checkout_get_actions(&actions, &counts, &data, workdir)) != 0)
		goto cleanup;

	data.total_steps = counts[CHECKOUT_ACTION__REMOVE] +
		counts[CHECKOUT_ACTION__REMOVE_CONFLICT] +
		counts[CHECKOUT_ACTION__UPDATE_BLOB] +
		counts[CHECKOUT_ACTION__UPDATE_SUBMODULE] +
		counts[CHECKOUT_ACTION__UPDATE_CONFLICT];

	report_progress(&data, NULL); /* establish 0 baseline */

	/* To deal with some order dependencies, perform remaining checkout
	 * in three passes: removes, then update blobs, then update submodules.
	 */
	if (counts[CHECKOUT_ACTION__REMOVE] > 0 &&
		(error = checkout_remove_the_old(actions, &data)) < 0)
		goto cleanup;

	if (counts[CHECKOUT_ACTION__REMOVE_CONFLICT] > 0 &&
		(error = checkout_remove_conflicts(&data)) < 0)
		goto cleanup;

	if (counts[CHECKOUT_ACTION__UPDATE_BLOB] > 0 &&
		(error = checkout_create_the_new(actions, &data)) < 0)
		goto cleanup;

	if (counts[CHECKOUT_ACTION__UPDATE_SUBMODULE] > 0 &&
		(error = checkout_create_submodules(actions, &data)) < 0)
		goto cleanup;

	if (counts[CHECKOUT_ACTION__UPDATE_CONFLICT] > 0 &&
		(error = checkout_create_conflicts(&data)) < 0)
		goto cleanup;

	if (data.index != git_iterator_index(target) &&
		(error = checkout_extensions_update_index(&data)) < 0)
		goto cleanup;

	assert(data.completed_steps == data.total_steps);

	if (data.opts.perfdata_cb)
		data.opts.perfdata_cb(&data.perfdata, data.opts.perfdata_payload);

cleanup:
	if (!error && data.index != NULL &&
		(data.strategy & CHECKOUT_INDEX_DONT_WRITE_MASK) == 0)
		error = git_index_write(data.index);

	git_diff_free(data.diff);
	git_iterator_free(workdir);
	git_iterator_free(baseline);
	git__free(actions);
	git__free(counts);
	checkout_data_clear(&data);

	return error;
}