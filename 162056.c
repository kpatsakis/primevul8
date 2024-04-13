static int nl80211_stop_p2p_device(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev = info->user_ptr[1];

	if (wdev->iftype != NL80211_IFTYPE_P2P_DEVICE)
		return -EOPNOTSUPP;

	if (!rdev->ops->stop_p2p_device)
		return -EOPNOTSUPP;

	if (!wdev->p2p_started)
		return 0;

	rdev_stop_p2p_device(rdev, wdev);
	wdev->p2p_started = false;

	mutex_lock(&rdev->devlist_mtx);
	rdev->opencount--;
	mutex_unlock(&rdev->devlist_mtx);

	if (WARN_ON(rdev->scan_req && rdev->scan_req->wdev == wdev)) {
		rdev->scan_req->aborted = true;
		___cfg80211_scan_done(rdev, true);
	}

	return 0;
}