__cfg80211_rdev_from_attrs(struct net *netns, struct nlattr **attrs)
{
	struct cfg80211_registered_device *rdev = NULL, *tmp;
	struct net_device *netdev;

	assert_cfg80211_lock();

	if (!attrs[NL80211_ATTR_WIPHY] &&
	    !attrs[NL80211_ATTR_IFINDEX] &&
	    !attrs[NL80211_ATTR_WDEV])
		return ERR_PTR(-EINVAL);

	if (attrs[NL80211_ATTR_WIPHY])
		rdev = cfg80211_rdev_by_wiphy_idx(
				nla_get_u32(attrs[NL80211_ATTR_WIPHY]));

	if (attrs[NL80211_ATTR_WDEV]) {
		u64 wdev_id = nla_get_u64(attrs[NL80211_ATTR_WDEV]);
		struct wireless_dev *wdev;
		bool found = false;

		tmp = cfg80211_rdev_by_wiphy_idx(wdev_id >> 32);
		if (tmp) {
			/* make sure wdev exists */
			mutex_lock(&tmp->devlist_mtx);
			list_for_each_entry(wdev, &tmp->wdev_list, list) {
				if (wdev->identifier != (u32)wdev_id)
					continue;
				found = true;
				break;
			}
			mutex_unlock(&tmp->devlist_mtx);

			if (!found)
				tmp = NULL;

			if (rdev && tmp != rdev)
				return ERR_PTR(-EINVAL);
			rdev = tmp;
		}
	}

	if (attrs[NL80211_ATTR_IFINDEX]) {
		int ifindex = nla_get_u32(attrs[NL80211_ATTR_IFINDEX]);
		netdev = dev_get_by_index(netns, ifindex);
		if (netdev) {
			if (netdev->ieee80211_ptr)
				tmp = wiphy_to_dev(
						netdev->ieee80211_ptr->wiphy);
			else
				tmp = NULL;

			dev_put(netdev);

			/* not wireless device -- return error */
			if (!tmp)
				return ERR_PTR(-EINVAL);

			/* mismatch -- return error */
			if (rdev && tmp != rdev)
				return ERR_PTR(-EINVAL);

			rdev = tmp;
		}
	}

	if (!rdev)
		return ERR_PTR(-ENODEV);

	if (netns != wiphy_net(&rdev->wiphy))
		return ERR_PTR(-ENODEV);

	return rdev;
}