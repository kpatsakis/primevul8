static int checkout_action_with_wd(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta,
	git_iterator *workdir,
	const git_index_entry *wd)
{
	*action = CHECKOUT_ACTION__NONE;

	switch (delta->status) {
	case GIT_DELTA_UNMODIFIED: /* case 14/15 or 33 */
		if (checkout_is_workdir_modified(data, &delta->old_file, &delta->new_file, wd)) {
			GIT_ERROR_CHECK_ERROR(
				checkout_notify(data, GIT_CHECKOUT_NOTIFY_DIRTY, delta, wd) );
			*action = CHECKOUT_ACTION_IF(FORCE, UPDATE_BLOB, NONE);
		}
		break;
	case GIT_DELTA_ADDED: /* case 3, 4 or 6 */
		if (git_iterator_current_is_ignored(workdir))
			*action = CHECKOUT_ACTION_IF(DONT_OVERWRITE_IGNORED, CONFLICT, UPDATE_BLOB);
		else
			*action = CHECKOUT_ACTION_IF(FORCE, UPDATE_BLOB, CONFLICT);
		break;
	case GIT_DELTA_DELETED: /* case 9 or 10 (or 26 but not really) */
		if (checkout_is_workdir_modified(data, &delta->old_file, &delta->new_file, wd))
			*action = CHECKOUT_ACTION_IF(FORCE, REMOVE, CONFLICT);
		else
			*action = CHECKOUT_ACTION_IF(SAFE, REMOVE, NONE);
		break;
	case GIT_DELTA_MODIFIED: /* case 16, 17, 18 (or 36 but not really) */
		if (wd->mode != GIT_FILEMODE_COMMIT &&
			checkout_is_workdir_modified(data, &delta->old_file, &delta->new_file, wd))
			*action = CHECKOUT_ACTION_IF(FORCE, UPDATE_BLOB, CONFLICT);
		else
			*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
		break;
	case GIT_DELTA_TYPECHANGE: /* case 22, 23, 29, 30 */
		if (delta->old_file.mode == GIT_FILEMODE_TREE) {
			if (wd->mode == GIT_FILEMODE_TREE)
				/* either deleting items in old tree will delete the wd dir,
				 * or we'll get a conflict when we attempt blob update...
				 */
				*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
			else if (wd->mode == GIT_FILEMODE_COMMIT) {
				/* workdir is possibly a "phantom" submodule - treat as a
				 * tree if the only submodule info came from the config
				 */
				if (submodule_is_config_only(data, wd->path))
					*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
				else
					*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
			} else
				*action = CHECKOUT_ACTION_IF(FORCE, REMOVE, CONFLICT);
		}
		else if (checkout_is_workdir_modified(data, &delta->old_file, &delta->new_file, wd))
			*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
		else
			*action = CHECKOUT_ACTION_IF(SAFE, REMOVE_AND_UPDATE, NONE);

		/* don't update if the typechange is to a tree */
		if (delta->new_file.mode == GIT_FILEMODE_TREE)
			*action = (*action & ~CHECKOUT_ACTION__UPDATE_BLOB);
		break;
	default: /* impossible */
		break;
	}

	return checkout_action_common(action, data, delta, wd);
}