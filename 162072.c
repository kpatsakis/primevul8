static bool nl80211_valid_auth_type(struct cfg80211_registered_device *rdev,
				    enum nl80211_auth_type auth_type,
				    enum nl80211_commands cmd)
{
	if (auth_type > NL80211_AUTHTYPE_MAX)
		return false;

	switch (cmd) {
	case NL80211_CMD_AUTHENTICATE:
		if (!(rdev->wiphy.features & NL80211_FEATURE_SAE) &&
		    auth_type == NL80211_AUTHTYPE_SAE)
			return false;
		return true;
	case NL80211_CMD_CONNECT:
	case NL80211_CMD_START_AP:
		/* SAE not supported yet */
		if (auth_type == NL80211_AUTHTYPE_SAE)
			return false;
		return true;
	default:
		return false;
	}
}