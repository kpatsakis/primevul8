nl80211_send_cqm_pktloss_notify(struct cfg80211_registered_device *rdev,
				struct net_device *netdev, const u8 *peer,
				u32 num_packets, gfp_t gfp)
{
	struct sk_buff *msg;
	struct nlattr *pinfoattr;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, gfp);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, NL80211_CMD_NOTIFY_CQM);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFINDEX, netdev->ifindex) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, peer))
		goto nla_put_failure;

	pinfoattr = nla_nest_start(msg, NL80211_ATTR_CQM);
	if (!pinfoattr)
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_ATTR_CQM_PKT_LOSS_EVENT, num_packets))
		goto nla_put_failure;

	nla_nest_end(msg, pinfoattr);

	genlmsg_end(msg, hdr);

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_mlme_mcgrp.id, gfp);
	return;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
}