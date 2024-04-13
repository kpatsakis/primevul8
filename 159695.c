static void bt_att_attach_chan(struct bt_att *att, struct bt_att_chan *chan)
{
	/* Push to head as EATT channels have higher priority */
	queue_push_head(att->chans, chan);
	chan->att = att;

	if (chan->mtu > att->mtu)
		att->mtu = chan->mtu;

	io_set_close_on_destroy(chan->io, att->close_on_unref);

	util_debug(att->debug_callback, att->debug_data, "Channel %p attached",
									chan);

	wakeup_chan_writer(chan, NULL);
}