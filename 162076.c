static int nl80211_set_cqm_rssi(struct genl_info *info,
				s32 threshold, u32 hysteresis)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev;
	struct net_device *dev = info->user_ptr[1];

	if (threshold > 0)
		return -EINVAL;

	wdev = dev->ieee80211_ptr;

	if (!rdev->ops->set_cqm_rssi_config)
		return -EOPNOTSUPP;

	if (wdev->iftype != NL80211_IFTYPE_STATION &&
	    wdev->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	return rdev_set_cqm_rssi_config(rdev, dev, threshold, hysteresis);
}