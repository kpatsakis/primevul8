bool bt_att_set_mtu(struct bt_att *att, uint16_t mtu)
{
	struct bt_att_chan *chan;
	void *buf;

	if (!att)
		return false;

	if (mtu < BT_ATT_DEFAULT_LE_MTU)
		return false;

	/* Original channel is always the last */
	chan = queue_peek_tail(att->chans);
	if (!chan)
		return -ENOTCONN;

	buf = malloc(mtu);
	if (!buf)
		return false;

	free(chan->buf);

	chan->mtu = mtu;
	chan->buf = buf;

	if (chan->mtu > att->mtu)
		att->mtu = chan->mtu;

	return true;
}