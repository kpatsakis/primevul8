static int nl80211_tdls_mgmt(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	u8 action_code, dialog_token;
	u16 status_code;
	u8 *peer;

	if (!(rdev->wiphy.flags & WIPHY_FLAG_SUPPORTS_TDLS) ||
	    !rdev->ops->tdls_mgmt)
		return -EOPNOTSUPP;

	if (!info->attrs[NL80211_ATTR_TDLS_ACTION] ||
	    !info->attrs[NL80211_ATTR_STATUS_CODE] ||
	    !info->attrs[NL80211_ATTR_TDLS_DIALOG_TOKEN] ||
	    !info->attrs[NL80211_ATTR_IE] ||
	    !info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	peer = nla_data(info->attrs[NL80211_ATTR_MAC]);
	action_code = nla_get_u8(info->attrs[NL80211_ATTR_TDLS_ACTION]);
	status_code = nla_get_u16(info->attrs[NL80211_ATTR_STATUS_CODE]);
	dialog_token = nla_get_u8(info->attrs[NL80211_ATTR_TDLS_DIALOG_TOKEN]);

	return rdev_tdls_mgmt(rdev, dev, peer, action_code,
			      dialog_token, status_code,
			      nla_data(info->attrs[NL80211_ATTR_IE]),
			      nla_len(info->attrs[NL80211_ATTR_IE]));
}