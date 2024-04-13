static bool nl80211_valid_wpa_versions(u32 wpa_versions)
{
	return !(wpa_versions & ~(NL80211_WPA_VERSION_1 |
				  NL80211_WPA_VERSION_2));
}