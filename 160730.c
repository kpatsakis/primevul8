static int checkout_create_conflicts(checkout_data *data)
{
	checkout_conflictdata *conflict;
	size_t i;
	int error = 0;

	git_vector_foreach(&data->update_conflicts, i, conflict) {

		/* Both deleted: nothing to do */
		if (conflict->ours == NULL && conflict->theirs == NULL)
			error = 0;

		else if ((data->strategy & GIT_CHECKOUT_USE_OURS) &&
			conflict->ours)
			error = checkout_write_entry(data, conflict, conflict->ours);
		else if ((data->strategy & GIT_CHECKOUT_USE_THEIRS) &&
			conflict->theirs)
			error = checkout_write_entry(data, conflict, conflict->theirs);

		/* Ignore the other side of name collisions. */
		else if ((data->strategy & GIT_CHECKOUT_USE_OURS) &&
			!conflict->ours && conflict->name_collision)
			error = 0;
		else if ((data->strategy & GIT_CHECKOUT_USE_THEIRS) &&
			!conflict->theirs && conflict->name_collision)
			error = 0;

		/* For modify/delete, name collisions and d/f conflicts, write
		 * the file (potentially with the name mangled.
		 */
		else if (conflict->ours != NULL && conflict->theirs == NULL)
			error = checkout_write_entry(data, conflict, conflict->ours);
		else if (conflict->ours == NULL && conflict->theirs != NULL)
			error = checkout_write_entry(data, conflict, conflict->theirs);

		/* Add/add conflicts and rename 1->2 conflicts, write the
		 * ours/theirs sides (potentially name mangled).
		 */
		else if (conflict->one_to_two)
			error = checkout_write_entries(data, conflict);

		/* If all sides are links, write the ours side */
		else if (S_ISLNK(conflict->ours->mode) &&
			S_ISLNK(conflict->theirs->mode))
			error = checkout_write_entry(data, conflict, conflict->ours);
		/* Link/file conflicts, write the file side */
		else if (S_ISLNK(conflict->ours->mode))
			error = checkout_write_entry(data, conflict, conflict->theirs);
		else if (S_ISLNK(conflict->theirs->mode))
			error = checkout_write_entry(data, conflict, conflict->ours);

		/* If any side is a gitlink, do nothing. */
		else if (conflict->submodule)
			error = 0;

		/* If any side is binary, write the ours side */
		else if (conflict->binary)
			error = checkout_write_entry(data, conflict, conflict->ours);

		else if (!error)
			error = checkout_write_merge(data, conflict);

		/* Update the index extensions (REUC and NAME) if we're checking
		 * out a different index. (Otherwise just leave them there.)
		 */
		if (!error && (data->strategy & GIT_CHECKOUT_DONT_UPDATE_INDEX) == 0)
			error = checkout_conflict_update_index(data, conflict);

		if (error)
			break;

		data->completed_steps++;
		report_progress(data,
			conflict->ours ? conflict->ours->path :
			(conflict->theirs ? conflict->theirs->path : conflict->ancestor->path));
	}

	return error;
}