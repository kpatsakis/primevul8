static int nl80211_stop_ap(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];

	return cfg80211_stop_ap(rdev, dev);
}