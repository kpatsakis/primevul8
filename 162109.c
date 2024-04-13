static int nl80211_deauthenticate(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	const u8 *ie = NULL, *bssid;
	int ie_len = 0;
	u16 reason_code;
	bool local_state_change;

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]))
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_REASON_CODE])
		return -EINVAL;

	if (!rdev->ops->deauth)
		return -EOPNOTSUPP;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	bssid = nla_data(info->attrs[NL80211_ATTR_MAC]);

	reason_code = nla_get_u16(info->attrs[NL80211_ATTR_REASON_CODE]);
	if (reason_code == 0) {
		/* Reason Code 0 is reserved */
		return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_IE]) {
		ie = nla_data(info->attrs[NL80211_ATTR_IE]);
		ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	local_state_change = !!info->attrs[NL80211_ATTR_LOCAL_STATE_CHANGE];

	return cfg80211_mlme_deauth(rdev, dev, bssid, ie, ie_len, reason_code,
				    local_state_change);
}