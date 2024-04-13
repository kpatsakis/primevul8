static int nl80211_del_station(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	u8 *mac_addr = NULL;

	if (info->attrs[NL80211_ATTR_MAC])
		mac_addr = nla_data(info->attrs[NL80211_ATTR_MAC]);

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP_VLAN &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_GO)
		return -EINVAL;

	if (!rdev->ops->del_station)
		return -EOPNOTSUPP;

	return rdev_del_station(rdev, dev, mac_addr);
}