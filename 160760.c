static int checkout_action_with_wd_dir(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta,
	git_iterator *workdir,
	const git_index_entry *wd)
{
	*action = CHECKOUT_ACTION__NONE;

	switch (delta->status) {
	case GIT_DELTA_UNMODIFIED: /* case 19 or 24 (or 34 but not really) */
		GIT_ERROR_CHECK_ERROR(
			checkout_notify(data, GIT_CHECKOUT_NOTIFY_DIRTY, delta, NULL));
		GIT_ERROR_CHECK_ERROR(
			checkout_notify(data, GIT_CHECKOUT_NOTIFY_UNTRACKED, NULL, wd));
		*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, NONE);
		break;
	case GIT_DELTA_ADDED:/* case 4 (and 7 for dir) */
	case GIT_DELTA_MODIFIED: /* case 20 (or 37 but not really) */
		if (delta->old_file.mode == GIT_FILEMODE_COMMIT)
			/* expected submodule (and maybe found one) */;
		else if (delta->new_file.mode != GIT_FILEMODE_TREE)
			*action = git_iterator_current_is_ignored(workdir) ?
				CHECKOUT_ACTION_IF(DONT_OVERWRITE_IGNORED, CONFLICT, REMOVE_AND_UPDATE) :
				CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
		break;
	case GIT_DELTA_DELETED: /* case 11 (and 27 for dir) */
		if (delta->old_file.mode != GIT_FILEMODE_TREE)
			GIT_ERROR_CHECK_ERROR(
				checkout_notify(data, GIT_CHECKOUT_NOTIFY_UNTRACKED, NULL, wd));
		break;
	case GIT_DELTA_TYPECHANGE: /* case 24 or 31 */
		if (delta->old_file.mode == GIT_FILEMODE_TREE) {
			/* For typechange from dir, remove dir and add blob, but it is
			 * not safe to remove dir if it contains modified files.
			 * However, safely removing child files will remove the parent
			 * directory if is it left empty, so we can defer removing the
			 * dir and it will succeed if no children are left.
			 */
			*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
		}
		else if (delta->new_file.mode != GIT_FILEMODE_TREE)
			/* For typechange to dir, dir is already created so no action */
			*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
		break;
	default: /* impossible */
		break;
	}

	return checkout_action_common(action, data, delta, wd);
}