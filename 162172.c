static int nl80211_set_tx_bitrate_mask(struct sk_buff *skb,
				       struct genl_info *info)
{
	struct nlattr *tb[NL80211_TXRATE_MAX + 1];
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct cfg80211_bitrate_mask mask;
	int rem, i;
	struct net_device *dev = info->user_ptr[1];
	struct nlattr *tx_rates;
	struct ieee80211_supported_band *sband;

	if (info->attrs[NL80211_ATTR_TX_RATES] == NULL)
		return -EINVAL;

	if (!rdev->ops->set_bitrate_mask)
		return -EOPNOTSUPP;

	memset(&mask, 0, sizeof(mask));
	/* Default to all rates enabled */
	for (i = 0; i < IEEE80211_NUM_BANDS; i++) {
		sband = rdev->wiphy.bands[i];
		mask.control[i].legacy =
			sband ? (1 << sband->n_bitrates) - 1 : 0;
		if (sband)
			memcpy(mask.control[i].mcs,
			       sband->ht_cap.mcs.rx_mask,
			       sizeof(mask.control[i].mcs));
		else
			memset(mask.control[i].mcs, 0,
			       sizeof(mask.control[i].mcs));
	}

	/*
	 * The nested attribute uses enum nl80211_band as the index. This maps
	 * directly to the enum ieee80211_band values used in cfg80211.
	 */
	BUILD_BUG_ON(NL80211_MAX_SUPP_HT_RATES > IEEE80211_HT_MCS_MASK_LEN * 8);
	nla_for_each_nested(tx_rates, info->attrs[NL80211_ATTR_TX_RATES], rem)
	{
		enum ieee80211_band band = nla_type(tx_rates);
		if (band < 0 || band >= IEEE80211_NUM_BANDS)
			return -EINVAL;
		sband = rdev->wiphy.bands[band];
		if (sband == NULL)
			return -EINVAL;
		nla_parse(tb, NL80211_TXRATE_MAX, nla_data(tx_rates),
			  nla_len(tx_rates), nl80211_txattr_policy);
		if (tb[NL80211_TXRATE_LEGACY]) {
			mask.control[band].legacy = rateset_to_mask(
				sband,
				nla_data(tb[NL80211_TXRATE_LEGACY]),
				nla_len(tb[NL80211_TXRATE_LEGACY]));
			if ((mask.control[band].legacy == 0) &&
			    nla_len(tb[NL80211_TXRATE_LEGACY]))
				return -EINVAL;
		}
		if (tb[NL80211_TXRATE_MCS]) {
			if (!ht_rateset_to_mask(
					sband,
					nla_data(tb[NL80211_TXRATE_MCS]),
					nla_len(tb[NL80211_TXRATE_MCS]),
					mask.control[band].mcs))
				return -EINVAL;
		}

		if (mask.control[band].legacy == 0) {
			/* don't allow empty legacy rates if HT
			 * is not even supported. */
			if (!rdev->wiphy.bands[band]->ht_cap.ht_supported)
				return -EINVAL;

			for (i = 0; i < IEEE80211_HT_MCS_MASK_LEN; i++)
				if (mask.control[band].mcs[i])
					break;

			/* legacy and mcs rates may not be both empty */
			if (i == IEEE80211_HT_MCS_MASK_LEN)
				return -EINVAL;
		}
	}

	return rdev_set_bitrate_mask(rdev, dev, NULL, &mask);
}