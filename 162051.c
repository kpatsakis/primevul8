static int nl80211_join_ibss(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct cfg80211_ibss_params ibss;
	struct wiphy *wiphy;
	struct cfg80211_cached_keys *connkeys = NULL;
	int err;

	memset(&ibss, 0, sizeof(ibss));

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]))
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_SSID] ||
	    !nla_len(info->attrs[NL80211_ATTR_SSID]))
		return -EINVAL;

	ibss.beacon_interval = 100;

	if (info->attrs[NL80211_ATTR_BEACON_INTERVAL]) {
		ibss.beacon_interval =
			nla_get_u32(info->attrs[NL80211_ATTR_BEACON_INTERVAL]);
		if (ibss.beacon_interval < 1 || ibss.beacon_interval > 10000)
			return -EINVAL;
	}

	if (!rdev->ops->join_ibss)
		return -EOPNOTSUPP;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_ADHOC)
		return -EOPNOTSUPP;

	wiphy = &rdev->wiphy;

	if (info->attrs[NL80211_ATTR_MAC]) {
		ibss.bssid = nla_data(info->attrs[NL80211_ATTR_MAC]);

		if (!is_valid_ether_addr(ibss.bssid))
			return -EINVAL;
	}
	ibss.ssid = nla_data(info->attrs[NL80211_ATTR_SSID]);
	ibss.ssid_len = nla_len(info->attrs[NL80211_ATTR_SSID]);

	if (info->attrs[NL80211_ATTR_IE]) {
		ibss.ie = nla_data(info->attrs[NL80211_ATTR_IE]);
		ibss.ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	err = nl80211_parse_chandef(rdev, info, &ibss.chandef);
	if (err)
		return err;

	if (!cfg80211_reg_can_beacon(&rdev->wiphy, &ibss.chandef))
		return -EINVAL;

	if (ibss.chandef.width > NL80211_CHAN_WIDTH_40)
		return -EINVAL;
	if (ibss.chandef.width != NL80211_CHAN_WIDTH_20_NOHT &&
	    !(rdev->wiphy.features & NL80211_FEATURE_HT_IBSS))
		return -EINVAL;

	ibss.channel_fixed = !!info->attrs[NL80211_ATTR_FREQ_FIXED];
	ibss.privacy = !!info->attrs[NL80211_ATTR_PRIVACY];

	if (info->attrs[NL80211_ATTR_BSS_BASIC_RATES]) {
		u8 *rates =
			nla_data(info->attrs[NL80211_ATTR_BSS_BASIC_RATES]);
		int n_rates =
			nla_len(info->attrs[NL80211_ATTR_BSS_BASIC_RATES]);
		struct ieee80211_supported_band *sband =
			wiphy->bands[ibss.chandef.chan->band];

		err = ieee80211_get_ratemask(sband, rates, n_rates,
					     &ibss.basic_rates);
		if (err)
			return err;
	}

	if (info->attrs[NL80211_ATTR_MCAST_RATE] &&
	    !nl80211_parse_mcast_rate(rdev, ibss.mcast_rate,
			nla_get_u32(info->attrs[NL80211_ATTR_MCAST_RATE])))
		return -EINVAL;

	if (ibss.privacy && info->attrs[NL80211_ATTR_KEYS]) {
		bool no_ht = false;

		connkeys = nl80211_parse_connkeys(rdev,
					  info->attrs[NL80211_ATTR_KEYS],
					  &no_ht);
		if (IS_ERR(connkeys))
			return PTR_ERR(connkeys);

		if ((ibss.chandef.width != NL80211_CHAN_WIDTH_20_NOHT) &&
		    no_ht) {
			kfree(connkeys);
			return -EINVAL;
		}
	}

	ibss.control_port =
		nla_get_flag(info->attrs[NL80211_ATTR_CONTROL_PORT]);

	err = cfg80211_join_ibss(rdev, dev, &ibss, connkeys);
	if (err)
		kfree(connkeys);
	return err;
}