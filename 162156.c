static int parse_station_flags(struct genl_info *info,
			       enum nl80211_iftype iftype,
			       struct station_parameters *params)
{
	struct nlattr *flags[NL80211_STA_FLAG_MAX + 1];
	struct nlattr *nla;
	int flag;

	/*
	 * Try parsing the new attribute first so userspace
	 * can specify both for older kernels.
	 */
	nla = info->attrs[NL80211_ATTR_STA_FLAGS2];
	if (nla) {
		struct nl80211_sta_flag_update *sta_flags;

		sta_flags = nla_data(nla);
		params->sta_flags_mask = sta_flags->mask;
		params->sta_flags_set = sta_flags->set;
		if ((params->sta_flags_mask |
		     params->sta_flags_set) & BIT(__NL80211_STA_FLAG_INVALID))
			return -EINVAL;
		return 0;
	}

	/* if present, parse the old attribute */

	nla = info->attrs[NL80211_ATTR_STA_FLAGS];
	if (!nla)
		return 0;

	if (nla_parse_nested(flags, NL80211_STA_FLAG_MAX,
			     nla, sta_flags_policy))
		return -EINVAL;

	/*
	 * Only allow certain flags for interface types so that
	 * other attributes are silently ignored. Remember that
	 * this is backward compatibility code with old userspace
	 * and shouldn't be hit in other cases anyway.
	 */
	switch (iftype) {
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_P2P_GO:
		params->sta_flags_mask = BIT(NL80211_STA_FLAG_AUTHORIZED) |
					 BIT(NL80211_STA_FLAG_SHORT_PREAMBLE) |
					 BIT(NL80211_STA_FLAG_WME) |
					 BIT(NL80211_STA_FLAG_MFP);
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_STATION:
		params->sta_flags_mask = BIT(NL80211_STA_FLAG_AUTHORIZED) |
					 BIT(NL80211_STA_FLAG_TDLS_PEER);
		break;
	case NL80211_IFTYPE_MESH_POINT:
		params->sta_flags_mask = BIT(NL80211_STA_FLAG_AUTHENTICATED) |
					 BIT(NL80211_STA_FLAG_MFP) |
					 BIT(NL80211_STA_FLAG_AUTHORIZED);
	default:
		return -EINVAL;
	}

	for (flag = 1; flag <= NL80211_STA_FLAG_MAX; flag++) {
		if (flags[flag]) {
			params->sta_flags_set |= (1<<flag);

			/* no longer support new API additions in old API */
			if (flag > NL80211_STA_FLAG_MAX_OLD_API)
				return -EINVAL;
		}
	}

	return 0;
}