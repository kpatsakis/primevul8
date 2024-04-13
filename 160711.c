static int checkout_action_with_wd_dir_empty(
	int *action,
	checkout_data *data,
	const git_diff_delta *delta)
{
	int error = checkout_action_no_wd(action, data, delta);

	/* We can always safely remove an empty directory. */
	if (error == 0 && *action != CHECKOUT_ACTION__NONE)
		*action |= CHECKOUT_ACTION__REMOVE;

	return error;
}