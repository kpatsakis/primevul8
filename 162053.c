void nl80211_send_conn_failed_event(struct cfg80211_registered_device *rdev,
				    struct net_device *dev, const u8 *mac_addr,
				    enum nl80211_connect_failed_reason reason,
				    gfp_t gfp)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_GOODSIZE, gfp);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, NL80211_CMD_CONN_FAILED);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr) ||
	    nla_put_u32(msg, NL80211_ATTR_CONN_FAILED_REASON, reason))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_mlme_mcgrp.id, gfp);
	return;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
}