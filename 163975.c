archive_read_open2(struct archive *a, void *client_data,
    archive_open_callback *client_opener,
    archive_read_callback *client_reader,
    archive_skip_callback *client_skipper,
    archive_close_callback *client_closer)
{
	/* Old archive_read_open2() is just a thin shell around
	 * archive_read_open1. */
	archive_read_set_callback_data(a, client_data);
	archive_read_set_open_callback(a, client_opener);
	archive_read_set_read_callback(a, client_reader);
	archive_read_set_skip_callback(a, client_skipper);
	archive_read_set_close_callback(a, client_closer);
	return archive_read_open1(a);
}