static int nl80211_parse_beacon(struct genl_info *info,
				struct cfg80211_beacon_data *bcn)
{
	bool haveinfo = false;

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_BEACON_TAIL]) ||
	    !is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]) ||
	    !is_valid_ie_attr(info->attrs[NL80211_ATTR_IE_PROBE_RESP]) ||
	    !is_valid_ie_attr(info->attrs[NL80211_ATTR_IE_ASSOC_RESP]))
		return -EINVAL;

	memset(bcn, 0, sizeof(*bcn));

	if (info->attrs[NL80211_ATTR_BEACON_HEAD]) {
		bcn->head = nla_data(info->attrs[NL80211_ATTR_BEACON_HEAD]);
		bcn->head_len = nla_len(info->attrs[NL80211_ATTR_BEACON_HEAD]);
		if (!bcn->head_len)
			return -EINVAL;
		haveinfo = true;
	}

	if (info->attrs[NL80211_ATTR_BEACON_TAIL]) {
		bcn->tail = nla_data(info->attrs[NL80211_ATTR_BEACON_TAIL]);
		bcn->tail_len =
		    nla_len(info->attrs[NL80211_ATTR_BEACON_TAIL]);
		haveinfo = true;
	}

	if (!haveinfo)
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_IE]) {
		bcn->beacon_ies = nla_data(info->attrs[NL80211_ATTR_IE]);
		bcn->beacon_ies_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	if (info->attrs[NL80211_ATTR_IE_PROBE_RESP]) {
		bcn->proberesp_ies =
			nla_data(info->attrs[NL80211_ATTR_IE_PROBE_RESP]);
		bcn->proberesp_ies_len =
			nla_len(info->attrs[NL80211_ATTR_IE_PROBE_RESP]);
	}

	if (info->attrs[NL80211_ATTR_IE_ASSOC_RESP]) {
		bcn->assocresp_ies =
			nla_data(info->attrs[NL80211_ATTR_IE_ASSOC_RESP]);
		bcn->assocresp_ies_len =
			nla_len(info->attrs[NL80211_ATTR_IE_ASSOC_RESP]);
	}

	if (info->attrs[NL80211_ATTR_PROBE_RESP]) {
		bcn->probe_resp =
			nla_data(info->attrs[NL80211_ATTR_PROBE_RESP]);
		bcn->probe_resp_len =
			nla_len(info->attrs[NL80211_ATTR_PROBE_RESP]);
	}

	return 0;
}