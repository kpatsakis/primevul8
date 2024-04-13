static int nl80211_start_p2p_device(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev = info->user_ptr[1];
	int err;

	if (!rdev->ops->start_p2p_device)
		return -EOPNOTSUPP;

	if (wdev->iftype != NL80211_IFTYPE_P2P_DEVICE)
		return -EOPNOTSUPP;

	if (wdev->p2p_started)
		return 0;

	mutex_lock(&rdev->devlist_mtx);
	err = cfg80211_can_add_interface(rdev, wdev->iftype);
	mutex_unlock(&rdev->devlist_mtx);
	if (err)
		return err;

	err = rdev_start_p2p_device(rdev, wdev);
	if (err)
		return err;

	wdev->p2p_started = true;
	mutex_lock(&rdev->devlist_mtx);
	rdev->opencount++;
	mutex_unlock(&rdev->devlist_mtx);

	return 0;
}