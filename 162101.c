static bool nl80211_get_ap_channel(struct cfg80211_registered_device *rdev,
				   struct cfg80211_ap_settings *params)
{
	struct wireless_dev *wdev;
	bool ret = false;

	mutex_lock(&rdev->devlist_mtx);

	list_for_each_entry(wdev, &rdev->wdev_list, list) {
		if (wdev->iftype != NL80211_IFTYPE_AP &&
		    wdev->iftype != NL80211_IFTYPE_P2P_GO)
			continue;

		if (!wdev->preset_chandef.chan)
			continue;

		params->chandef = wdev->preset_chandef;
		ret = true;
		break;
	}

	mutex_unlock(&rdev->devlist_mtx);

	return ret;
}