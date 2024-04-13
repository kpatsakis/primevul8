int bt_att_get_security(struct bt_att *att, uint8_t *enc_size)
{
	struct bt_att_chan *chan;
	int ret;

	if (!att)
		return -EINVAL;

	chan = queue_peek_tail(att->chans);
	if (!chan)
		return -ENOTCONN;

	ret = bt_att_chan_get_security(chan);
	if (ret < 0)
		return ret;

	if (enc_size)
		*enc_size = att->enc_size;

	return ret;
}