static int nl80211_send_scan_msg(struct sk_buff *msg,
				 struct cfg80211_registered_device *rdev,
				 struct wireless_dev *wdev,
				 u32 portid, u32 seq, int flags,
				 u32 cmd)
{
	void *hdr;

	hdr = nl80211hdr_put(msg, portid, seq, flags, cmd);
	if (!hdr)
		return -1;

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    (wdev->netdev && nla_put_u32(msg, NL80211_ATTR_IFINDEX,
					 wdev->netdev->ifindex)) ||
	    nla_put_u64(msg, NL80211_ATTR_WDEV, wdev_id(wdev)))
		goto nla_put_failure;

	/* ignore errors and send incomplete event anyway */
	nl80211_add_scan_req(msg, rdev);

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}