static int nl80211_start_ap(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_ap_settings params;
	int err;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_GO)
		return -EOPNOTSUPP;

	if (!rdev->ops->start_ap)
		return -EOPNOTSUPP;

	if (wdev->beacon_interval)
		return -EALREADY;

	memset(&params, 0, sizeof(params));

	/* these are required for START_AP */
	if (!info->attrs[NL80211_ATTR_BEACON_INTERVAL] ||
	    !info->attrs[NL80211_ATTR_DTIM_PERIOD] ||
	    !info->attrs[NL80211_ATTR_BEACON_HEAD])
		return -EINVAL;

	err = nl80211_parse_beacon(info, &params.beacon);
	if (err)
		return err;

	params.beacon_interval =
		nla_get_u32(info->attrs[NL80211_ATTR_BEACON_INTERVAL]);
	params.dtim_period =
		nla_get_u32(info->attrs[NL80211_ATTR_DTIM_PERIOD]);

	err = cfg80211_validate_beacon_int(rdev, params.beacon_interval);
	if (err)
		return err;

	/*
	 * In theory, some of these attributes should be required here
	 * but since they were not used when the command was originally
	 * added, keep them optional for old user space programs to let
	 * them continue to work with drivers that do not need the
	 * additional information -- drivers must check!
	 */
	if (info->attrs[NL80211_ATTR_SSID]) {
		params.ssid = nla_data(info->attrs[NL80211_ATTR_SSID]);
		params.ssid_len =
			nla_len(info->attrs[NL80211_ATTR_SSID]);
		if (params.ssid_len == 0 ||
		    params.ssid_len > IEEE80211_MAX_SSID_LEN)
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_HIDDEN_SSID]) {
		params.hidden_ssid = nla_get_u32(
			info->attrs[NL80211_ATTR_HIDDEN_SSID]);
		if (params.hidden_ssid != NL80211_HIDDEN_SSID_NOT_IN_USE &&
		    params.hidden_ssid != NL80211_HIDDEN_SSID_ZERO_LEN &&
		    params.hidden_ssid != NL80211_HIDDEN_SSID_ZERO_CONTENTS)
			return -EINVAL;
	}

	params.privacy = !!info->attrs[NL80211_ATTR_PRIVACY];

	if (info->attrs[NL80211_ATTR_AUTH_TYPE]) {
		params.auth_type = nla_get_u32(
			info->attrs[NL80211_ATTR_AUTH_TYPE]);
		if (!nl80211_valid_auth_type(rdev, params.auth_type,
					     NL80211_CMD_START_AP))
			return -EINVAL;
	} else
		params.auth_type = NL80211_AUTHTYPE_AUTOMATIC;

	err = nl80211_crypto_settings(rdev, info, &params.crypto,
				      NL80211_MAX_NR_CIPHER_SUITES);
	if (err)
		return err;

	if (info->attrs[NL80211_ATTR_INACTIVITY_TIMEOUT]) {
		if (!(rdev->wiphy.features & NL80211_FEATURE_INACTIVITY_TIMER))
			return -EOPNOTSUPP;
		params.inactivity_timeout = nla_get_u16(
			info->attrs[NL80211_ATTR_INACTIVITY_TIMEOUT]);
	}

	if (info->attrs[NL80211_ATTR_P2P_CTWINDOW]) {
		if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_GO)
			return -EINVAL;
		params.p2p_ctwindow =
			nla_get_u8(info->attrs[NL80211_ATTR_P2P_CTWINDOW]);
		if (params.p2p_ctwindow > 127)
			return -EINVAL;
		if (params.p2p_ctwindow != 0 &&
		    !(rdev->wiphy.features & NL80211_FEATURE_P2P_GO_CTWIN))
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_P2P_OPPPS]) {
		u8 tmp;

		if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_GO)
			return -EINVAL;
		tmp = nla_get_u8(info->attrs[NL80211_ATTR_P2P_OPPPS]);
		if (tmp > 1)
			return -EINVAL;
		params.p2p_opp_ps = tmp;
		if (params.p2p_opp_ps != 0 &&
		    !(rdev->wiphy.features & NL80211_FEATURE_P2P_GO_OPPPS))
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_WIPHY_FREQ]) {
		err = nl80211_parse_chandef(rdev, info, &params.chandef);
		if (err)
			return err;
	} else if (wdev->preset_chandef.chan) {
		params.chandef = wdev->preset_chandef;
	} else if (!nl80211_get_ap_channel(rdev, &params))
		return -EINVAL;

	if (!cfg80211_reg_can_beacon(&rdev->wiphy, &params.chandef))
		return -EINVAL;

	mutex_lock(&rdev->devlist_mtx);
	err = cfg80211_can_use_chan(rdev, wdev, params.chandef.chan,
				    CHAN_MODE_SHARED);
	mutex_unlock(&rdev->devlist_mtx);

	if (err)
		return err;

	err = rdev_start_ap(rdev, dev, &params);
	if (!err) {
		wdev->preset_chandef = params.chandef;
		wdev->beacon_interval = params.beacon_interval;
		wdev->channel = params.chandef.chan;
		wdev->ssid_len = params.ssid_len;
		memcpy(wdev->ssid, params.ssid, wdev->ssid_len);
	}
	return err;
}