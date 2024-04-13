static int nl80211_del_mpath(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	u8 *dst = NULL;

	if (info->attrs[NL80211_ATTR_MAC])
		dst = nla_data(info->attrs[NL80211_ATTR_MAC]);

	if (!rdev->ops->del_mpath)
		return -EOPNOTSUPP;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT)
		return -EOPNOTSUPP;

	return rdev_del_mpath(rdev, dev, dst);
}