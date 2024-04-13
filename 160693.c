static int checkout_create_submodules(
	unsigned int *actions,
	checkout_data *data)
{
	int error = 0;
	git_diff_delta *delta;
	size_t i;

	git_vector_foreach(&data->diff->deltas, i, delta) {
		if (actions[i] & CHECKOUT_ACTION__DEFER_REMOVE) {
			/* this has a blocker directory that should only be removed iff
			 * all of the contents of the directory were safely removed
			 */
			if ((error = checkout_deferred_remove(
					data->repo, delta->old_file.path)) < 0)
				return error;
		}

		if (actions[i] & CHECKOUT_ACTION__UPDATE_SUBMODULE) {
			int error = checkout_submodule(data, &delta->new_file);
			if (error < 0)
				return error;

			data->completed_steps++;
			report_progress(data, delta->new_file.path);
		}
	}

	return 0;
}