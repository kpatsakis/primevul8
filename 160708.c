static int checkout_get_actions(
	uint32_t **actions_ptr,
	size_t **counts_ptr,
	checkout_data *data,
	git_iterator *workdir)
{
	int error = 0, act;
	const git_index_entry *wditem;
	git_vector pathspec = GIT_VECTOR_INIT, *deltas;
	git_pool pathpool;
	git_diff_delta *delta;
	size_t i, *counts = NULL;
	uint32_t *actions = NULL;

	git_pool_init(&pathpool, 1);

	if (data->opts.paths.count > 0 &&
		git_pathspec__vinit(&pathspec, &data->opts.paths, &pathpool) < 0)
		return -1;

	if ((error = git_iterator_current(&wditem, workdir)) < 0 &&
		error != GIT_ITEROVER)
		goto fail;

	deltas = &data->diff->deltas;

	*counts_ptr = counts = git__calloc(CHECKOUT_ACTION__MAX+1, sizeof(size_t));
	*actions_ptr = actions = git__calloc(
		deltas->length ? deltas->length : 1, sizeof(uint32_t));
	if (!counts || !actions) {
		error = -1;
		goto fail;
	}

	git_vector_foreach(deltas, i, delta) {
		if ((error = checkout_action(&act, data, delta, workdir, &wditem, &pathspec)) == 0)
			error = checkout_verify_paths(data->repo, act, delta);

		if (error != 0)
			goto fail;

		actions[i] = act;

		if (act & CHECKOUT_ACTION__REMOVE)
			counts[CHECKOUT_ACTION__REMOVE]++;
		if (act & CHECKOUT_ACTION__UPDATE_BLOB)
			counts[CHECKOUT_ACTION__UPDATE_BLOB]++;
		if (act & CHECKOUT_ACTION__UPDATE_SUBMODULE)
			counts[CHECKOUT_ACTION__UPDATE_SUBMODULE]++;
		if (act & CHECKOUT_ACTION__CONFLICT)
			counts[CHECKOUT_ACTION__CONFLICT]++;
	}

	error = checkout_remaining_wd_items(data, workdir, wditem, &pathspec);
	if (error)
		goto fail;

	counts[CHECKOUT_ACTION__REMOVE] += data->removes.length;

	if (counts[CHECKOUT_ACTION__CONFLICT] > 0 &&
		(data->strategy & GIT_CHECKOUT_ALLOW_CONFLICTS) == 0)
	{
		git_error_set(GIT_ERROR_CHECKOUT, "%"PRIuZ" %s checkout",
			counts[CHECKOUT_ACTION__CONFLICT],
			counts[CHECKOUT_ACTION__CONFLICT] == 1 ?
			"conflict prevents" : "conflicts prevent");
		error = GIT_ECONFLICT;
		goto fail;
	}


	if ((error = checkout_get_remove_conflicts(data, workdir, &pathspec)) < 0 ||
		(error = checkout_get_update_conflicts(data, workdir, &pathspec)) < 0)
		goto fail;

	counts[CHECKOUT_ACTION__REMOVE_CONFLICT] = git_vector_length(&data->remove_conflicts);
	counts[CHECKOUT_ACTION__UPDATE_CONFLICT] = git_vector_length(&data->update_conflicts);

	git_pathspec__vfree(&pathspec);
	git_pool_clear(&pathpool);

	return 0;

fail:
	*counts_ptr = NULL;
	git__free(counts);
	*actions_ptr = NULL;
	git__free(actions);

	git_pathspec__vfree(&pathspec);
	git_pool_clear(&pathpool);

	return error;
}