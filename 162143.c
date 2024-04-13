void nl80211_send_scan_done(struct cfg80211_registered_device *rdev,
			    struct wireless_dev *wdev)
{
	struct sk_buff *msg;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return;

	if (nl80211_send_scan_msg(msg, rdev, wdev, 0, 0, 0,
				  NL80211_CMD_NEW_SCAN_RESULTS) < 0) {
		nlmsg_free(msg);
		return;
	}

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_scan_mcgrp.id, GFP_KERNEL);
}