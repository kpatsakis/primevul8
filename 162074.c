static int nl80211_set_mcast_rate(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	int mcast_rate[IEEE80211_NUM_BANDS];
	u32 nla_rate;
	int err;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_ADHOC &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT)
		return -EOPNOTSUPP;

	if (!rdev->ops->set_mcast_rate)
		return -EOPNOTSUPP;

	memset(mcast_rate, 0, sizeof(mcast_rate));

	if (!info->attrs[NL80211_ATTR_MCAST_RATE])
		return -EINVAL;

	nla_rate = nla_get_u32(info->attrs[NL80211_ATTR_MCAST_RATE]);
	if (!nl80211_parse_mcast_rate(rdev, mcast_rate, nla_rate))
		return -EINVAL;

	err = rdev->ops->set_mcast_rate(&rdev->wiphy, dev, mcast_rate);

	return err;
}