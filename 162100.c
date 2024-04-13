static int nl80211_connect(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct cfg80211_connect_params connect;
	struct wiphy *wiphy;
	struct cfg80211_cached_keys *connkeys = NULL;
	int err;

	memset(&connect, 0, sizeof(connect));

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]))
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_SSID] ||
	    !nla_len(info->attrs[NL80211_ATTR_SSID]))
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_AUTH_TYPE]) {
		connect.auth_type =
			nla_get_u32(info->attrs[NL80211_ATTR_AUTH_TYPE]);
		if (!nl80211_valid_auth_type(rdev, connect.auth_type,
					     NL80211_CMD_CONNECT))
			return -EINVAL;
	} else
		connect.auth_type = NL80211_AUTHTYPE_AUTOMATIC;

	connect.privacy = info->attrs[NL80211_ATTR_PRIVACY];

	err = nl80211_crypto_settings(rdev, info, &connect.crypto,
				      NL80211_MAX_NR_CIPHER_SUITES);
	if (err)
		return err;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	wiphy = &rdev->wiphy;

	connect.bg_scan_period = -1;
	if (info->attrs[NL80211_ATTR_BG_SCAN_PERIOD] &&
		(wiphy->flags & WIPHY_FLAG_SUPPORTS_FW_ROAM)) {
		connect.bg_scan_period =
			nla_get_u16(info->attrs[NL80211_ATTR_BG_SCAN_PERIOD]);
	}

	if (info->attrs[NL80211_ATTR_MAC])
		connect.bssid = nla_data(info->attrs[NL80211_ATTR_MAC]);
	connect.ssid = nla_data(info->attrs[NL80211_ATTR_SSID]);
	connect.ssid_len = nla_len(info->attrs[NL80211_ATTR_SSID]);

	if (info->attrs[NL80211_ATTR_IE]) {
		connect.ie = nla_data(info->attrs[NL80211_ATTR_IE]);
		connect.ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	if (info->attrs[NL80211_ATTR_WIPHY_FREQ]) {
		connect.channel =
			ieee80211_get_channel(wiphy,
			    nla_get_u32(info->attrs[NL80211_ATTR_WIPHY_FREQ]));
		if (!connect.channel ||
		    connect.channel->flags & IEEE80211_CHAN_DISABLED)
			return -EINVAL;
	}

	if (connect.privacy && info->attrs[NL80211_ATTR_KEYS]) {
		connkeys = nl80211_parse_connkeys(rdev,
					  info->attrs[NL80211_ATTR_KEYS], NULL);
		if (IS_ERR(connkeys))
			return PTR_ERR(connkeys);
	}

	if (nla_get_flag(info->attrs[NL80211_ATTR_DISABLE_HT]))
		connect.flags |= ASSOC_REQ_DISABLE_HT;

	if (info->attrs[NL80211_ATTR_HT_CAPABILITY_MASK])
		memcpy(&connect.ht_capa_mask,
		       nla_data(info->attrs[NL80211_ATTR_HT_CAPABILITY_MASK]),
		       sizeof(connect.ht_capa_mask));

	if (info->attrs[NL80211_ATTR_HT_CAPABILITY]) {
		if (!info->attrs[NL80211_ATTR_HT_CAPABILITY_MASK]) {
			kfree(connkeys);
			return -EINVAL;
		}
		memcpy(&connect.ht_capa,
		       nla_data(info->attrs[NL80211_ATTR_HT_CAPABILITY]),
		       sizeof(connect.ht_capa));
	}

	err = cfg80211_connect(rdev, dev, &connect, connkeys);
	if (err)
		kfree(connkeys);
	return err;
}