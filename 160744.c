static int checkout_remaining_wd_items(
	checkout_data *data,
	git_iterator *workdir,
	const git_index_entry *wd,
	git_vector *spec)
{
	int error = 0;

	while (wd && !error)
		error = checkout_action_wd_only(data, workdir, &wd, spec);

	if (error == GIT_ITEROVER)
		error = 0;

	return error;
}