cfg80211_get_dev_from_info(struct net *netns, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev;

	mutex_lock(&cfg80211_mutex);
	rdev = __cfg80211_rdev_from_attrs(netns, info->attrs);

	/* if it is not an error we grab the lock on
	 * it to assure it won't be going away while
	 * we operate on it */
	if (!IS_ERR(rdev))
		mutex_lock(&rdev->mtx);

	mutex_unlock(&cfg80211_mutex);

	return rdev;
}