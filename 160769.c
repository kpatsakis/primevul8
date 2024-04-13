static int checkout_action_with_wd_blocker(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta,
	const git_index_entry *wd)
{
	*action = CHECKOUT_ACTION__NONE;

	switch (delta->status) {
	case GIT_DELTA_UNMODIFIED:
		/* should show delta as dirty / deleted */
		GIT_ERROR_CHECK_ERROR(
			checkout_notify(data, GIT_CHECKOUT_NOTIFY_DIRTY, delta, wd) );
		*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, NONE);
		break;
	case GIT_DELTA_ADDED:
	case GIT_DELTA_MODIFIED:
		*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
		break;
	case GIT_DELTA_DELETED:
		*action = CHECKOUT_ACTION_IF(FORCE, REMOVE, CONFLICT);
		break;
	case GIT_DELTA_TYPECHANGE:
		/* not 100% certain about this... */
		*action = CHECKOUT_ACTION_IF(FORCE, REMOVE_AND_UPDATE, CONFLICT);
		break;
	default: /* impossible */
		break;
	}

	return checkout_action_common(action, data, delta, wd);
}