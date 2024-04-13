static int nl80211_parse_mesh_setup(struct genl_info *info,
				     struct mesh_setup *setup)
{
	struct nlattr *tb[NL80211_MESH_SETUP_ATTR_MAX + 1];

	if (!info->attrs[NL80211_ATTR_MESH_SETUP])
		return -EINVAL;
	if (nla_parse_nested(tb, NL80211_MESH_SETUP_ATTR_MAX,
			     info->attrs[NL80211_ATTR_MESH_SETUP],
			     nl80211_mesh_setup_params_policy))
		return -EINVAL;

	if (tb[NL80211_MESH_SETUP_ENABLE_VENDOR_SYNC])
		setup->sync_method =
		(nla_get_u8(tb[NL80211_MESH_SETUP_ENABLE_VENDOR_SYNC])) ?
		 IEEE80211_SYNC_METHOD_VENDOR :
		 IEEE80211_SYNC_METHOD_NEIGHBOR_OFFSET;

	if (tb[NL80211_MESH_SETUP_ENABLE_VENDOR_PATH_SEL])
		setup->path_sel_proto =
		(nla_get_u8(tb[NL80211_MESH_SETUP_ENABLE_VENDOR_PATH_SEL])) ?
		 IEEE80211_PATH_PROTOCOL_VENDOR :
		 IEEE80211_PATH_PROTOCOL_HWMP;

	if (tb[NL80211_MESH_SETUP_ENABLE_VENDOR_METRIC])
		setup->path_metric =
		(nla_get_u8(tb[NL80211_MESH_SETUP_ENABLE_VENDOR_METRIC])) ?
		 IEEE80211_PATH_METRIC_VENDOR :
		 IEEE80211_PATH_METRIC_AIRTIME;


	if (tb[NL80211_MESH_SETUP_IE]) {
		struct nlattr *ieattr =
			tb[NL80211_MESH_SETUP_IE];
		if (!is_valid_ie_attr(ieattr))
			return -EINVAL;
		setup->ie = nla_data(ieattr);
		setup->ie_len = nla_len(ieattr);
	}
	setup->is_authenticated = nla_get_flag(tb[NL80211_MESH_SETUP_USERSPACE_AUTH]);
	setup->is_secure = nla_get_flag(tb[NL80211_MESH_SETUP_USERSPACE_AMPE]);

	return 0;
}