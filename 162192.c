__cfg80211_wdev_from_attrs(struct net *netns, struct nlattr **attrs)
{
	struct cfg80211_registered_device *rdev;
	struct wireless_dev *result = NULL;
	bool have_ifidx = attrs[NL80211_ATTR_IFINDEX];
	bool have_wdev_id = attrs[NL80211_ATTR_WDEV];
	u64 wdev_id;
	int wiphy_idx = -1;
	int ifidx = -1;

	assert_cfg80211_lock();

	if (!have_ifidx && !have_wdev_id)
		return ERR_PTR(-EINVAL);

	if (have_ifidx)
		ifidx = nla_get_u32(attrs[NL80211_ATTR_IFINDEX]);
	if (have_wdev_id) {
		wdev_id = nla_get_u64(attrs[NL80211_ATTR_WDEV]);
		wiphy_idx = wdev_id >> 32;
	}

	list_for_each_entry(rdev, &cfg80211_rdev_list, list) {
		struct wireless_dev *wdev;

		if (wiphy_net(&rdev->wiphy) != netns)
			continue;

		if (have_wdev_id && rdev->wiphy_idx != wiphy_idx)
			continue;

		mutex_lock(&rdev->devlist_mtx);
		list_for_each_entry(wdev, &rdev->wdev_list, list) {
			if (have_ifidx && wdev->netdev &&
			    wdev->netdev->ifindex == ifidx) {
				result = wdev;
				break;
			}
			if (have_wdev_id && wdev->identifier == (u32)wdev_id) {
				result = wdev;
				break;
			}
		}
		mutex_unlock(&rdev->devlist_mtx);

		if (result)
			break;
	}

	if (result)
		return result;
	return ERR_PTR(-ENODEV);
}