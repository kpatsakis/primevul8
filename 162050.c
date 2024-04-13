static int nl80211_new_station(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int err;
	struct net_device *dev = info->user_ptr[1];
	struct station_parameters params;
	u8 *mac_addr = NULL;

	memset(&params, 0, sizeof(params));

	if (!info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_STA_LISTEN_INTERVAL])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_STA_AID])
		return -EINVAL;

	mac_addr = nla_data(info->attrs[NL80211_ATTR_MAC]);
	params.supported_rates =
		nla_data(info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES]);
	params.supported_rates_len =
		nla_len(info->attrs[NL80211_ATTR_STA_SUPPORTED_RATES]);
	params.listen_interval =
		nla_get_u16(info->attrs[NL80211_ATTR_STA_LISTEN_INTERVAL]);

	params.aid = nla_get_u16(info->attrs[NL80211_ATTR_STA_AID]);
	if (!params.aid || params.aid > IEEE80211_MAX_AID)
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_HT_CAPABILITY])
		params.ht_capa =
			nla_data(info->attrs[NL80211_ATTR_HT_CAPABILITY]);

	if (info->attrs[NL80211_ATTR_VHT_CAPABILITY])
		params.vht_capa =
			nla_data(info->attrs[NL80211_ATTR_VHT_CAPABILITY]);

	if (info->attrs[NL80211_ATTR_STA_PLINK_ACTION])
		params.plink_action =
		    nla_get_u8(info->attrs[NL80211_ATTR_STA_PLINK_ACTION]);

	if (!rdev->ops->add_station)
		return -EOPNOTSUPP;

	if (parse_station_flags(info, dev->ieee80211_ptr->iftype, &params))
		return -EINVAL;

	switch (dev->ieee80211_ptr->iftype) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_P2P_GO:
		/* parse WME attributes if sta is WME capable */
		if ((rdev->wiphy.flags & WIPHY_FLAG_AP_UAPSD) &&
		    (params.sta_flags_set & BIT(NL80211_STA_FLAG_WME)) &&
		    info->attrs[NL80211_ATTR_STA_WME]) {
			struct nlattr *tb[NL80211_STA_WME_MAX + 1];
			struct nlattr *nla;

			nla = info->attrs[NL80211_ATTR_STA_WME];
			err = nla_parse_nested(tb, NL80211_STA_WME_MAX, nla,
					       nl80211_sta_wme_policy);
			if (err)
				return err;

			if (tb[NL80211_STA_WME_UAPSD_QUEUES])
				params.uapsd_queues =
				     nla_get_u8(tb[NL80211_STA_WME_UAPSD_QUEUES]);
			if (params.uapsd_queues &
					~IEEE80211_WMM_IE_STA_QOSINFO_AC_MASK)
				return -EINVAL;

			if (tb[NL80211_STA_WME_MAX_SP])
				params.max_sp =
				     nla_get_u8(tb[NL80211_STA_WME_MAX_SP]);

			if (params.max_sp &
					~IEEE80211_WMM_IE_STA_QOSINFO_SP_MASK)
				return -EINVAL;

			params.sta_modify_mask |= STATION_PARAM_APPLY_UAPSD;
		}
		/* TDLS peers cannot be added */
		if (params.sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER))
			return -EINVAL;
		/* but don't bother the driver with it */
		params.sta_flags_mask &= ~BIT(NL80211_STA_FLAG_TDLS_PEER);

		/* allow authenticated/associated only if driver handles it */
		if (!(rdev->wiphy.features &
				NL80211_FEATURE_FULL_AP_CLIENT_STATE) &&
		    params.sta_flags_mask &
				(BIT(NL80211_STA_FLAG_AUTHENTICATED) |
				 BIT(NL80211_STA_FLAG_ASSOCIATED)))
			return -EINVAL;

		/* must be last in here for error handling */
		params.vlan = get_vlan(info, rdev);
		if (IS_ERR(params.vlan))
			return PTR_ERR(params.vlan);
		break;
	case NL80211_IFTYPE_MESH_POINT:
		/* associated is disallowed */
		if (params.sta_flags_mask & BIT(NL80211_STA_FLAG_ASSOCIATED))
			return -EINVAL;
		/* TDLS peers cannot be added */
		if (params.sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER))
			return -EINVAL;
		break;
	case NL80211_IFTYPE_STATION:
		/* associated is disallowed */
		if (params.sta_flags_mask & BIT(NL80211_STA_FLAG_ASSOCIATED))
			return -EINVAL;
		/* Only TDLS peers can be added */
		if (!(params.sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER)))
			return -EINVAL;
		/* Can only add if TDLS ... */
		if (!(rdev->wiphy.flags & WIPHY_FLAG_SUPPORTS_TDLS))
			return -EOPNOTSUPP;
		/* ... with external setup is supported */
		if (!(rdev->wiphy.flags & WIPHY_FLAG_TDLS_EXTERNAL_SETUP))
			return -EOPNOTSUPP;
		break;
	default:
		return -EOPNOTSUPP;
	}

	/* be aware of params.vlan when changing code here */

	err = rdev_add_station(rdev, dev, mac_addr, &params);

	if (params.vlan)
		dev_put(params.vlan);
	return err;
}