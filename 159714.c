bool bt_att_set_security(struct bt_att *att, int level)
{
	struct bt_att_chan *chan;

	if (!att || level < BT_ATT_SECURITY_AUTO ||
						level > BT_ATT_SECURITY_HIGH)
		return false;

	chan = queue_peek_tail(att->chans);
	if (!chan)
		return -ENOTCONN;

	return bt_att_chan_set_security(chan, level);
}