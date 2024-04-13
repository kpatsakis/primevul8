static int nl80211_send_iface(struct sk_buff *msg, u32 portid, u32 seq, int flags,
			      struct cfg80211_registered_device *rdev,
			      struct wireless_dev *wdev)
{
	struct net_device *dev = wdev->netdev;
	void *hdr;

	hdr = nl80211hdr_put(msg, portid, seq, flags, NL80211_CMD_NEW_INTERFACE);
	if (!hdr)
		return -1;

	if (dev &&
	    (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	     nla_put_string(msg, NL80211_ATTR_IFNAME, dev->name)))
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFTYPE, wdev->iftype) ||
	    nla_put_u64(msg, NL80211_ATTR_WDEV, wdev_id(wdev)) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, wdev_address(wdev)) ||
	    nla_put_u32(msg, NL80211_ATTR_GENERATION,
			rdev->devlist_generation ^
			(cfg80211_rdev_list_generation << 2)))
		goto nla_put_failure;

	if (rdev->ops->get_channel) {
		int ret;
		struct cfg80211_chan_def chandef;

		ret = rdev_get_channel(rdev, wdev, &chandef);
		if (ret == 0) {
			if (nl80211_send_chandef(msg, &chandef))
				goto nla_put_failure;
		}
	}

	if (wdev->ssid_len) {
		if (nla_put(msg, NL80211_ATTR_SSID, wdev->ssid_len, wdev->ssid))
			goto nla_put_failure;
	}

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}