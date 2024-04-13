static bool __nl80211_unexpected_frame(struct net_device *dev, u8 cmd,
				       const u8 *addr, gfp_t gfp)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_registered_device *rdev = wiphy_to_dev(wdev->wiphy);
	struct sk_buff *msg;
	void *hdr;
	int err;
	u32 nlportid = ACCESS_ONCE(wdev->ap_unexpected_nlportid);

	if (!nlportid)
		return false;

	msg = nlmsg_new(100, gfp);
	if (!msg)
		return true;

	hdr = nl80211hdr_put(msg, 0, 0, 0, cmd);
	if (!hdr) {
		nlmsg_free(msg);
		return true;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, addr))
		goto nla_put_failure;

	err = genlmsg_end(msg, hdr);
	if (err < 0) {
		nlmsg_free(msg);
		return true;
	}

	genlmsg_unicast(wiphy_net(&rdev->wiphy), msg, nlportid);
	return true;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
	return true;
}