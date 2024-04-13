static int checkout_conflict_update_index(
	checkout_data *data,
	checkout_conflictdata *conflict)
{
	int error = 0;

	if (conflict->ancestor)
		error = checkout_conflict_add(data, conflict->ancestor);

	if (!error && conflict->ours)
		error = checkout_conflict_add(data, conflict->ours);

	if (!error && conflict->theirs)
		error = checkout_conflict_add(data, conflict->theirs);

	return error;
}