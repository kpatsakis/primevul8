static void nl80211_send_remain_on_chan_event(
	int cmd, struct cfg80211_registered_device *rdev,
	struct wireless_dev *wdev, u64 cookie,
	struct ieee80211_channel *chan,
	unsigned int duration, gfp_t gfp)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, gfp);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, cmd);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    (wdev->netdev && nla_put_u32(msg, NL80211_ATTR_IFINDEX,
					 wdev->netdev->ifindex)) ||
	    nla_put_u64(msg, NL80211_ATTR_WDEV, wdev_id(wdev)) ||
	    nla_put_u32(msg, NL80211_ATTR_WIPHY_FREQ, chan->center_freq) ||
	    nla_put_u32(msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE,
			NL80211_CHAN_NO_HT) ||
	    nla_put_u64(msg, NL80211_ATTR_COOKIE, cookie))
		goto nla_put_failure;

	if (cmd == NL80211_CMD_REMAIN_ON_CHANNEL &&
	    nla_put_u32(msg, NL80211_ATTR_DURATION, duration))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_mlme_mcgrp.id, gfp);
	return;

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
}