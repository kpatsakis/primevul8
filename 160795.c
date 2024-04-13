static void checkout_data_clear(checkout_data *data)
{
	if (data->opts_free_baseline) {
		git_tree_free(data->opts.baseline);
		data->opts.baseline = NULL;
	}

	git_vector_free(&data->removes);
	git_pool_clear(&data->pool);

	git_vector_free_deep(&data->remove_conflicts);
	git_vector_free_deep(&data->update_conflicts);

	git__free(data->pfx);
	data->pfx = NULL;

	git_buf_dispose(&data->target_path);
	git_buf_dispose(&data->tmp);

	git_index_free(data->index);
	data->index = NULL;

	git_strmap_free(data->mkdir_map);
	data->mkdir_map = NULL;

	git_attr_session__free(&data->attr_session);
}