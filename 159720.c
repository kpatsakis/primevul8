static bool change_security(struct bt_att_chan *chan, uint8_t ecode)
{
	int security;

	if (chan->sec_level != BT_ATT_SECURITY_AUTO)
		return false;

	security = bt_att_chan_get_security(chan);

	if (ecode == BT_ATT_ERROR_INSUFFICIENT_ENCRYPTION &&
					security < BT_ATT_SECURITY_MEDIUM) {
		security = BT_ATT_SECURITY_MEDIUM;
	} else if (ecode == BT_ATT_ERROR_AUTHENTICATION) {
		if (security < BT_ATT_SECURITY_MEDIUM)
			security = BT_ATT_SECURITY_MEDIUM;
		else if (security < BT_ATT_SECURITY_HIGH)
			security = BT_ATT_SECURITY_HIGH;
		else if (security < BT_ATT_SECURITY_FIPS)
			security = BT_ATT_SECURITY_FIPS;
		else
			return false;
	} else {
		return false;
	}

	return bt_att_chan_set_security(chan, security);
}