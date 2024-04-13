static bool bt_att_chan_set_security(struct bt_att_chan *chan, int level)
{
	struct bt_security sec;

	if (chan->type == BT_ATT_LOCAL) {
		chan->sec_level = level;
		return true;
	}

	memset(&sec, 0, sizeof(sec));
	sec.level = level;

	if (setsockopt(chan->fd, SOL_BLUETOOTH, BT_SECURITY, &sec,
							sizeof(sec)) < 0)
		return false;

	return true;
}