static int nl80211_setdel_pmksa(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int (*rdev_ops)(struct wiphy *wiphy, struct net_device *dev,
			struct cfg80211_pmksa *pmksa) = NULL;
	struct net_device *dev = info->user_ptr[1];
	struct cfg80211_pmksa pmksa;

	memset(&pmksa, 0, sizeof(struct cfg80211_pmksa));

	if (!info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_PMKID])
		return -EINVAL;

	pmksa.pmkid = nla_data(info->attrs[NL80211_ATTR_PMKID]);
	pmksa.bssid = nla_data(info->attrs[NL80211_ATTR_MAC]);

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	switch (info->genlhdr->cmd) {
	case NL80211_CMD_SET_PMKSA:
		rdev_ops = rdev->ops->set_pmksa;
		break;
	case NL80211_CMD_DEL_PMKSA:
		rdev_ops = rdev->ops->del_pmksa;
		break;
	default:
		WARN_ON(1);
		break;
	}

	if (!rdev_ops)
		return -EOPNOTSUPP;

	return rdev_ops(&rdev->wiphy, dev, &pmksa);
}