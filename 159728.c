static int bt_att_chan_get_security(struct bt_att_chan *chan)
{
	struct bt_security sec;
	socklen_t len;

	if (chan->type == BT_ATT_LOCAL)
		return chan->sec_level;

	memset(&sec, 0, sizeof(sec));
	len = sizeof(sec);
	if (getsockopt(chan->fd, SOL_BLUETOOTH, BT_SECURITY, &sec, &len) < 0)
		return -EIO;

	return sec.level;
}