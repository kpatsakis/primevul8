static int checkout_extensions_update_index(checkout_data *data)
{
	const git_index_reuc_entry *reuc_entry;
	const git_index_name_entry *name_entry;
	size_t i;
	int error = 0;

	if ((data->strategy & GIT_CHECKOUT_UPDATE_ONLY) != 0)
		return 0;

	if (data->update_reuc) {
		git_vector_foreach(data->update_reuc, i, reuc_entry) {
			if ((error = git_index_reuc_add(data->index, reuc_entry->path,
				reuc_entry->mode[0], &reuc_entry->oid[0],
				reuc_entry->mode[1], &reuc_entry->oid[1],
				reuc_entry->mode[2], &reuc_entry->oid[2])) < 0)
				goto done;
		}
	}

	if (data->update_names) {
		git_vector_foreach(data->update_names, i, name_entry) {
			if ((error = git_index_name_add(data->index, name_entry->ancestor,
				name_entry->ours, name_entry->theirs)) < 0)
				goto done;
		}
	}

done:
	return error;
}