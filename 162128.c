void nl80211_notify_dev_rename(struct cfg80211_registered_device *rdev)
{
	struct sk_buff *msg;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return;

	if (nl80211_send_wiphy(msg, 0, 0, 0, rdev) < 0) {
		nlmsg_free(msg);
		return;
	}

	genlmsg_multicast_netns(wiphy_net(&rdev->wiphy), msg, 0,
				nl80211_config_mcgrp.id, GFP_KERNEL);
}