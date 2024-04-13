void nl80211_send_disconnected(struct cfg80211_registered_device *rdev,
			       struct net_device *netdev, u16 reason,
			       const u8 *ie, size_t ie_len, bool from_ap)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, NL80211_CMD_DISCONNECT);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFINDEX, netdev->ifindex) ||
	    (from_ap && reason &&
	     nla_put_u16(msg, NL80211_ATTR_REASON_CODE, reason)) ||
	    (from_ap &&
	     nla_put_flag(msg, NL80211_ATTR_DISCONNECTED_BY_AP)) ||
	    (ie && nla_put(msg, NL80211_ATTR_IE, ie_len, ie)))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_mlme_mcgrp.id, GFP_KERNEL);
	return;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);

}