
static void io_rsrc_data_free(struct io_rsrc_data *data)
{
	size_t size = data->nr * sizeof(data->tags[0][0]);

	if (data->tags)
		io_free_page_table((void **)data->tags, size);
	kfree(data);