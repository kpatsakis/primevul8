static void wakeup_writer(struct bt_att *att)
{
	queue_foreach(att->chans, wakeup_chan_writer, NULL);
}