uint8_t bt_att_get_link_type(struct bt_att *att)
{
	struct bt_att_chan *chan;

	if (!att)
		return -EINVAL;

	chan = queue_peek_tail(att->chans);
	if (!chan)
		return -ENOTCONN;

	return chan->type;
}