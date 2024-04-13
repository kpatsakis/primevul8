static int nl80211_associate(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct cfg80211_crypto_settings crypto;
	struct ieee80211_channel *chan;
	const u8 *bssid, *ssid, *ie = NULL, *prev_bssid = NULL;
	int err, ssid_len, ie_len = 0;
	bool use_mfp = false;
	u32 flags = 0;
	struct ieee80211_ht_cap *ht_capa = NULL;
	struct ieee80211_ht_cap *ht_capa_mask = NULL;

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]))
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_MAC] ||
	    !info->attrs[NL80211_ATTR_SSID] ||
	    !info->attrs[NL80211_ATTR_WIPHY_FREQ])
		return -EINVAL;

	if (!rdev->ops->assoc)
		return -EOPNOTSUPP;

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION &&
	    dev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_CLIENT)
		return -EOPNOTSUPP;

	bssid = nla_data(info->attrs[NL80211_ATTR_MAC]);

	chan = ieee80211_get_channel(&rdev->wiphy,
		nla_get_u32(info->attrs[NL80211_ATTR_WIPHY_FREQ]));
	if (!chan || (chan->flags & IEEE80211_CHAN_DISABLED))
		return -EINVAL;

	ssid = nla_data(info->attrs[NL80211_ATTR_SSID]);
	ssid_len = nla_len(info->attrs[NL80211_ATTR_SSID]);

	if (info->attrs[NL80211_ATTR_IE]) {
		ie = nla_data(info->attrs[NL80211_ATTR_IE]);
		ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	if (info->attrs[NL80211_ATTR_USE_MFP]) {
		enum nl80211_mfp mfp =
			nla_get_u32(info->attrs[NL80211_ATTR_USE_MFP]);
		if (mfp == NL80211_MFP_REQUIRED)
			use_mfp = true;
		else if (mfp != NL80211_MFP_NO)
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_PREV_BSSID])
		prev_bssid = nla_data(info->attrs[NL80211_ATTR_PREV_BSSID]);

	if (nla_get_flag(info->attrs[NL80211_ATTR_DISABLE_HT]))
		flags |= ASSOC_REQ_DISABLE_HT;

	if (info->attrs[NL80211_ATTR_HT_CAPABILITY_MASK])
		ht_capa_mask =
			nla_data(info->attrs[NL80211_ATTR_HT_CAPABILITY_MASK]);

	if (info->attrs[NL80211_ATTR_HT_CAPABILITY]) {
		if (!ht_capa_mask)
			return -EINVAL;
		ht_capa = nla_data(info->attrs[NL80211_ATTR_HT_CAPABILITY]);
	}

	err = nl80211_crypto_settings(rdev, info, &crypto, 1);
	if (!err)
		err = cfg80211_mlme_assoc(rdev, dev, chan, bssid, prev_bssid,
					  ssid, ssid_len, ie, ie_len, use_mfp,
					  &crypto, flags, ht_capa,
					  ht_capa_mask);

	return err;
}