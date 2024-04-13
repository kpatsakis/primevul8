static int nl80211_set_power_save(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev;
	struct net_device *dev = info->user_ptr[1];
	u8 ps_state;
	bool state;
	int err;

	if (!info->attrs[NL80211_ATTR_PS_STATE])
		return -EINVAL;

	ps_state = nla_get_u32(info->attrs[NL80211_ATTR_PS_STATE]);

	if (ps_state != NL80211_PS_DISABLED && ps_state != NL80211_PS_ENABLED)
		return -EINVAL;

	wdev = dev->ieee80211_ptr;

	if (!rdev->ops->set_power_mgmt)
		return -EOPNOTSUPP;

	state = (ps_state == NL80211_PS_ENABLED) ? true : false;

	if (state == wdev->ps)
		return 0;

	err = rdev_set_power_mgmt(rdev, dev, state, wdev->ps_timeout);
	if (!err)
		wdev->ps = state;
	return err;
}