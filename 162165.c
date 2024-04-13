static int nl80211_set_station(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int err;
	struct net_device *dev = info->user_ptr[1];
	struct station_parameters params;
	u8 *mac_addr = NULL;

	memset(&params, 0, sizeof(params));

	params.listen_interval = -1;
	params.plink_state = -1;

	if (info->attrs[NL80211_ATTR_STA_AID])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	mac_addr = nla_data(info->attrs[NL80211_ATTR_MAC]);

	if (info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES]) {
		params.supported_rates =
			nla_data(info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES]);
		params.supported_rates_len =
			nla_len(info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES]);
	}

	if (info->attrs[NL80211_ATTR_STA_LISTEN_INTERVAL] ||
	    info->attrs[NL80211_ATTR_HT_CAPABILITY])
		return -EINVAL;

	if (!rdev->ops->change_station)
		return -EOPNOTSUPP;

	if (parse_station_flags(info, dev->ieee80211_ptr->iftype, &params))
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_STA_PLINK_ACTION])
		params.plink_action =
		    nla_get_u8(info->attrs[NL80211_ATTR_STA_PLINK_ACTION]);

	if (info->attrs[NL80211_ATTR_STA_PLINK_STATE])
		params.plink_state =
		    nla_get_u8(info->attrs[NL80211_ATTR_STA_PLINK_STATE]);

	if (info->attrs[NL80211_ATTR_LOCAL_MESH_POWER_MODE]) {
		enum nl80211_mesh_power_mode pm = nla_get_u32(
			info->attrs[NL80211_ATTR_LOCAL_MESH_POWER_MODE]);

		if (pm <= NL80211_MESH_POWER_UNKNOWN ||
		    pm > NL80211_MESH_POWER_MAX)
			return -EINVAL;

		params.local_pm = pm;
	}

	switch (dev->ieee80211_ptr->iftype) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_P2P_GO:
		/* disallow mesh-specific things */
		if (params.plink_action)
			return -EINVAL;
		if (params.local_pm)
			return -EINVAL;

		/* TDLS can't be set, ... */
		if (params.sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER))
			return -EINVAL;
		/*
		 * ... but don't bother the driver with it. This works around
		 * a hostapd/wpa_supplicant issue -- it always includes the
		 * TLDS_PEER flag in the mask even for AP mode.
		 */
		params.sta_flags_mask &= ~BIT(NL80211_STA_FLAG_TDLS_PEER);

		/* accept only the listed bits */
		if (params.sta_flags_mask &
				~(BIT(NL80211_STA_FLAG_AUTHORIZED) |
				  BIT(NL80211_STA_FLAG_AUTHENTICATED) |
				  BIT(NL80211_STA_FLAG_ASSOCIATED) |
				  BIT(NL80211_STA_FLAG_SHORT_PREAMBLE) |
				  BIT(NL80211_STA_FLAG_WME) |
				  BIT(NL80211_STA_FLAG_MFP)))
			return -EINVAL;

		/* but authenticated/associated only if driver handles it */
		if (!(rdev->wiphy.features &
				NL80211_FEATURE_FULL_AP_CLIENT_STATE) &&
		    params.sta_flags_mask &
				(BIT(NL80211_STA_FLAG_AUTHENTICATED) |
				 BIT(NL80211_STA_FLAG_ASSOCIATED)))
			return -EINVAL;

		/* reject other things that can't change */
		if (params.supported_rates)
			return -EINVAL;

		/* must be last in here for error handling */
		params.vlan = get_vlan(info, rdev);
		if (IS_ERR(params.vlan))
			return PTR_ERR(params.vlan);
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_STATION:
		/*
		 * Don't allow userspace to change the TDLS_PEER flag,
		 * but silently ignore attempts to change it since we
		 * don't have state here to verify that it doesn't try
		 * to change the flag.
		 */
		params.sta_flags_mask &= ~BIT(NL80211_STA_FLAG_TDLS_PEER);
		/* fall through */
	case NL80211_IFTYPE_ADHOC:
		/* disallow things sta doesn't support */
		if (params.plink_action)
			return -EINVAL;
		if (params.local_pm)
			return -EINVAL;
		/* reject any changes other than AUTHORIZED */
		if (params.sta_flags_mask & ~BIT(NL80211_STA_FLAG_AUTHORIZED))
			return -EINVAL;
		break;
	case NL80211_IFTYPE_MESH_POINT:
		/* disallow things mesh doesn't support */
		if (params.vlan)
			return -EINVAL;
		if (params.supported_rates)
			return -EINVAL;
		/*
		 * No special handling for TDLS here -- the userspace
		 * mesh code doesn't have this bug.
		 */
		if (params.sta_flags_mask &
				~(BIT(NL80211_STA_FLAG_AUTHENTICATED) |
				  BIT(NL80211_STA_FLAG_MFP) |
				  BIT(NL80211_STA_FLAG_AUTHORIZED)))
			return -EINVAL;
		break;
	default:
		return -EOPNOTSUPP;
	}

	/* be aware of params.vlan when changing code here */

	err = rdev_change_station(rdev, dev, mac_addr, &params);

	if (params.vlan)
		dev_put(params.vlan);

	return err;
}