static int nl80211_stop_sched_scan(struct sk_buff *skb,
				   struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int err;

	if (!(rdev->wiphy.flags & WIPHY_FLAG_SUPPORTS_SCHED_SCAN) ||
	    !rdev->ops->sched_scan_stop)
		return -EOPNOTSUPP;

	mutex_lock(&rdev->sched_scan_mtx);
	err = __cfg80211_stop_sched_scan(rdev, false);
	mutex_unlock(&rdev->sched_scan_mtx);

	return err;
}