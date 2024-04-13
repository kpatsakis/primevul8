static int nl80211_set_cqm_txe(struct genl_info *info,
			       u32 rate, u32 pkts, u32 intvl)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev;
	struct net_device *dev = info->user_ptr[1];

	if (rate > 100 || intvl > NL80211_CQM_TXE_MAX_INTVL)
		return -EINVAL;

	wdev = dev->ieee80211_ptr;

	if (!rdev->ops->set_cqm_txe_config)
		return -EOPNOTSUPP;

	if (wdev->iftype != NL80211_IFTYPE_STATION &&
	    wdev->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	return rdev_set_cqm_txe_config(rdev, dev, rate, pkts, intvl);
}