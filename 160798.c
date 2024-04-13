static int checkout_remove_conflicts(checkout_data *data)
{
	const char *conflict;
	size_t i;

	git_vector_foreach(&data->remove_conflicts, i, conflict) {
		if (git_index_conflict_remove(data->index, conflict) < 0)
			return -1;

		data->completed_steps++;
	}

	return 0;
}