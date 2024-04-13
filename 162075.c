void cfg80211_tdls_oper_request(struct net_device *dev, const u8 *peer,
				enum nl80211_tdls_operation oper,
				u16 reason_code, gfp_t gfp)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_registered_device *rdev = wiphy_to_dev(wdev->wiphy);
	struct sk_buff *msg;
	void *hdr;
	int err;

	trace_cfg80211_tdls_oper_request(wdev->wiphy, dev, peer, oper,
					 reason_code);

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, gfp);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, NL80211_CMD_TDLS_OPER);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	    nla_put_u8(msg, NL80211_ATTR_TDLS_OPERATION, oper) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, peer) ||
	    (reason_code > 0 &&
	     nla_put_u16(msg, NL80211_ATTR_REASON_CODE, reason_code)))
		goto nla_put_failure;

	err = genlmsg_end(msg, hdr);
	if (err < 0) {
		nlmsg_free(msg);
		return;
	}

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_mlme_mcgrp.id, gfp);
	return;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
}