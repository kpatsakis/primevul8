static int checkout_action_common(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta,
	const git_index_entry *wd)
{
	git_checkout_notify_t notify = GIT_CHECKOUT_NOTIFY_NONE;

	if ((data->strategy & GIT_CHECKOUT_UPDATE_ONLY) != 0)
		*action = (*action & ~CHECKOUT_ACTION__REMOVE);

	if ((*action & CHECKOUT_ACTION__UPDATE_BLOB) != 0) {
		if (S_ISGITLINK(delta->new_file.mode))
			*action = (*action & ~CHECKOUT_ACTION__UPDATE_BLOB) |
				CHECKOUT_ACTION__UPDATE_SUBMODULE;

		/* to "update" a symlink, we must remove the old one first */
		if (delta->new_file.mode == GIT_FILEMODE_LINK && wd != NULL)
			*action |= CHECKOUT_ACTION__REMOVE;

		/* if the file is on disk and doesn't match our mode, force update */
		if (wd &&
			GIT_PERMS_IS_EXEC(wd->mode) !=
			GIT_PERMS_IS_EXEC(delta->new_file.mode))
				*action |= CHECKOUT_ACTION__REMOVE;

		notify = GIT_CHECKOUT_NOTIFY_UPDATED;
	}

	if ((*action & CHECKOUT_ACTION__CONFLICT) != 0)
		notify = GIT_CHECKOUT_NOTIFY_CONFLICT;

	return checkout_notify(data, notify, delta, wd);
}