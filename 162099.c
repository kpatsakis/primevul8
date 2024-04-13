static int __nl80211_set_channel(struct cfg80211_registered_device *rdev,
				 struct wireless_dev *wdev,
				 struct genl_info *info)
{
	struct cfg80211_chan_def chandef;
	int result;
	enum nl80211_iftype iftype = NL80211_IFTYPE_MONITOR;

	if (wdev)
		iftype = wdev->iftype;

	if (!nl80211_can_set_dev_channel(wdev))
		return -EOPNOTSUPP;

	result = nl80211_parse_chandef(rdev, info, &chandef);
	if (result)
		return result;

	mutex_lock(&rdev->devlist_mtx);
	switch (iftype) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_P2P_GO:
		if (wdev->beacon_interval) {
			result = -EBUSY;
			break;
		}
		if (!cfg80211_reg_can_beacon(&rdev->wiphy, &chandef)) {
			result = -EINVAL;
			break;
		}
		wdev->preset_chandef = chandef;
		result = 0;
		break;
	case NL80211_IFTYPE_MESH_POINT:
		result = cfg80211_set_mesh_channel(rdev, wdev, &chandef);
		break;
	case NL80211_IFTYPE_MONITOR:
		result = cfg80211_set_monitor_channel(rdev, &chandef);
		break;
	default:
		result = -EINVAL;
	}
	mutex_unlock(&rdev->devlist_mtx);

	return result;
}