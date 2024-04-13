int bt_att_get_fd(struct bt_att *att)
{
	struct bt_att_chan *chan;

	if (!att)
		return -1;

	if (queue_isempty(att->chans))
		return -ENOTCONN;

	chan = queue_peek_tail(att->chans);

	return chan->fd;
}