static int nl80211_key_allowed(struct wireless_dev *wdev)
{
	ASSERT_WDEV_LOCK(wdev);

	switch (wdev->iftype) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_P2P_GO:
	case NL80211_IFTYPE_MESH_POINT:
		break;
	case NL80211_IFTYPE_ADHOC:
		if (!wdev->current_bss)
			return -ENOLINK;
		break;
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
		if (wdev->sme_state != CFG80211_SME_CONNECTED)
			return -ENOLINK;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}