static void write_watch_destroy(void *user_data)
{
	struct bt_att_chan *chan = user_data;

	chan->writer_active = false;
}