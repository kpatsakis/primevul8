static int checkout_write_entries(
	checkout_data *data,
	checkout_conflictdata *conflict)
{
	int error = 0;

	if ((error = checkout_write_entry(data, conflict, conflict->ours)) >= 0)
		error = checkout_write_entry(data, conflict, conflict->theirs);

	return error;
}