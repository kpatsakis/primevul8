static int checkout_action_no_wd(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta)
{
	int error = 0;

	*action = CHECKOUT_ACTION__NONE;

	switch (delta->status) {
	case GIT_DELTA_UNMODIFIED: /* case 12 */
		error = checkout_notify(data, GIT_CHECKOUT_NOTIFY_DIRTY, delta, NULL);
		if (error)
			return error;
		*action = CHECKOUT_ACTION_IF(RECREATE_MISSING, UPDATE_BLOB, NONE);
		break;
	case GIT_DELTA_ADDED:    /* case 2 or 28 (and 5 but not really) */
		*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
		break;
	case GIT_DELTA_MODIFIED: /* case 13 (and 35 but not really) */
		*action = CHECKOUT_ACTION_IF(RECREATE_MISSING, UPDATE_BLOB, CONFLICT);
		break;
	case GIT_DELTA_TYPECHANGE: /* case 21 (B->T) and 28 (T->B)*/
		if (delta->new_file.mode == GIT_FILEMODE_TREE)
			*action = CHECKOUT_ACTION_IF(SAFE, UPDATE_BLOB, NONE);
		break;
	case GIT_DELTA_DELETED: /* case 8 or 25 */
		*action = CHECKOUT_ACTION_IF(SAFE, REMOVE, NONE);
		break;
	default: /* impossible */
		break;
	}

	return checkout_action_common(action, data, delta, NULL);
}