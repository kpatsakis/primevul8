static int checkout_queue_remove(checkout_data *data, const char *path)
{
	char *copy = git_pool_strdup(&data->pool, path);
	GIT_ERROR_CHECK_ALLOC(copy);
	return git_vector_insert(&data->removes, copy);
}