static int nl80211_set_beacon(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_beacon_data params;
	int err;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_GO)
		return -EOPNOTSUPP;

	if (!rdev->ops->change_beacon)
		return -EOPNOTSUPP;

	if (!wdev->beacon_interval)
		return -EINVAL;

	err = nl80211_parse_beacon(info, &params);
	if (err)
		return err;

	return rdev_change_beacon(rdev, dev, &params);
}