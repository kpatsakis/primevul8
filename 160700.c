static int checkout_action_wd_only(
	checkout_data *data,
	git_iterator *workdir,
	const git_index_entry **wditem,
	git_vector *pathspec)
{
	int error = 0;
	bool remove = false;
	git_checkout_notify_t notify = GIT_CHECKOUT_NOTIFY_NONE;
	const git_index_entry *wd = *wditem;

	if (!git_pathspec__match(
			pathspec, wd->path,
			(data->strategy & GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH) != 0,
			git_iterator_ignore_case(workdir), NULL, NULL))
		return git_iterator_advance(wditem, workdir);

	/* check if item is tracked in the index but not in the checkout diff */
	if (data->index != NULL) {
		size_t pos;

		error = git_index__find_pos(
			&pos, data->index, wd->path, 0, GIT_INDEX_STAGE_ANY);

		if (wd->mode != GIT_FILEMODE_TREE) {
			if (!error) { /* found by git_index__find_pos call */
				notify = GIT_CHECKOUT_NOTIFY_DIRTY;
				remove = ((data->strategy & GIT_CHECKOUT_FORCE) != 0);
			} else if (error != GIT_ENOTFOUND)
				return error;
			else
				error = 0; /* git_index__find_pos does not set error msg */
		} else {
			/* for tree entries, we have to see if there are any index
			 * entries that are contained inside that tree
			 */
			const git_index_entry *e = git_index_get_byindex(data->index, pos);

			if (e != NULL && data->diff->pfxcomp(e->path, wd->path) == 0)
				return git_iterator_advance_into(wditem, workdir);
		}
	}

	if (notify != GIT_CHECKOUT_NOTIFY_NONE) {
		/* if we found something in the index, notify and advance */
		if ((error = checkout_notify(data, notify, NULL, wd)) != 0)
			return error;

		if (remove && wd_item_is_removable(data, wd))
			error = checkout_queue_remove(data, wd->path);

		if (!error)
			error = git_iterator_advance(wditem, workdir);
	} else {
		/* untracked or ignored - can't know which until we advance through */
		bool over = false, removable = wd_item_is_removable(data, wd);
		git_iterator_status_t untracked_state;

		/* copy the entry for issuing notification callback later */
		git_index_entry saved_wd = *wd;
		git_buf_sets(&data->tmp, wd->path);
		saved_wd.path = data->tmp.ptr;

		error = git_iterator_advance_over(
			wditem, &untracked_state, workdir);
		if (error == GIT_ITEROVER)
			over = true;
		else if (error < 0)
			return error;

		if (untracked_state == GIT_ITERATOR_STATUS_IGNORED) {
			notify = GIT_CHECKOUT_NOTIFY_IGNORED;
			remove = ((data->strategy & GIT_CHECKOUT_REMOVE_IGNORED) != 0);
		} else {
			notify = GIT_CHECKOUT_NOTIFY_UNTRACKED;
			remove = ((data->strategy & GIT_CHECKOUT_REMOVE_UNTRACKED) != 0);
		}

		if ((error = checkout_notify(data, notify, NULL, &saved_wd)) != 0)
			return error;

		if (remove && removable)
			error = checkout_queue_remove(data, saved_wd.path);

		if (!error && over) /* restore ITEROVER if needed */
			error = GIT_ITEROVER;
	}

	return error;
}