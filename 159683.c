int bt_att_attach_fd(struct bt_att *att, int fd)
{
	struct bt_att_chan *chan;

	if (!att || fd < 0)
		return -EINVAL;

	chan = bt_att_chan_new(fd, BT_ATT_EATT);
	if (!chan)
		return -EINVAL;

	bt_att_attach_chan(att, chan);

	return 0;
}