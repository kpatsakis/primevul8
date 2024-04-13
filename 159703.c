struct bt_att *bt_att_new(int fd, bool ext_signed)
{
	struct bt_att *att;
	struct bt_att_chan *chan;

	chan = bt_att_chan_new(fd, io_get_type(fd));
	if (!chan)
		return NULL;

	att = new0(struct bt_att, 1);
	att->chans = queue_new();
	att->mtu = chan->mtu;

	/* crypto is optional, if not available leave it NULL */
	if (!ext_signed)
		att->crypto = bt_crypto_new();

	att->req_queue = queue_new();
	att->ind_queue = queue_new();
	att->write_queue = queue_new();
	att->notify_list = queue_new();
	att->disconn_list = queue_new();

	bt_att_attach_chan(att, chan);

	return bt_att_ref(att);
}