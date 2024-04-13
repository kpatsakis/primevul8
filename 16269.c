
static void io_wait_rsrc_data(struct io_rsrc_data *data)
{
	if (data && !atomic_dec_and_test(&data->refs))
		wait_for_completion(&data->done);