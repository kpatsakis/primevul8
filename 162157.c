static int nl80211_parse_mesh_config(struct genl_info *info,
				     struct mesh_config *cfg,
				     u32 *mask_out)
{
	struct nlattr *tb[NL80211_MESHCONF_ATTR_MAX + 1];
	u32 mask = 0;

#define FILL_IN_MESH_PARAM_IF_SET(tb, cfg, param, min, max, mask, attr, fn) \
do {									    \
	if (tb[attr]) {							    \
		if (fn(tb[attr]) < min || fn(tb[attr]) > max)		    \
			return -EINVAL;					    \
		cfg->param = fn(tb[attr]);				    \
		mask |= (1 << (attr - 1));				    \
	}								    \
} while (0)


	if (!info->attrs[NL80211_ATTR_MESH_CONFIG])
		return -EINVAL;
	if (nla_parse_nested(tb, NL80211_MESHCONF_ATTR_MAX,
			     info->attrs[NL80211_ATTR_MESH_CONFIG],
			     nl80211_meshconf_params_policy))
		return -EINVAL;

	/* This makes sure that there aren't more than 32 mesh config
	 * parameters (otherwise our bitfield scheme would not work.) */
	BUILD_BUG_ON(NL80211_MESHCONF_ATTR_MAX > 32);

	/* Fill in the params struct */
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshRetryTimeout, 1, 255,
				  mask, NL80211_MESHCONF_RETRY_TIMEOUT,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshConfirmTimeout, 1, 255,
				  mask, NL80211_MESHCONF_CONFIRM_TIMEOUT,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHoldingTimeout, 1, 255,
				  mask, NL80211_MESHCONF_HOLDING_TIMEOUT,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshMaxPeerLinks, 0, 255,
				  mask, NL80211_MESHCONF_MAX_PEER_LINKS,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshMaxRetries, 0, 16,
				  mask, NL80211_MESHCONF_MAX_RETRIES,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshTTL, 1, 255,
				  mask, NL80211_MESHCONF_TTL, nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, element_ttl, 1, 255,
				  mask, NL80211_MESHCONF_ELEMENT_TTL,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, auto_open_plinks, 0, 1,
				  mask, NL80211_MESHCONF_AUTO_OPEN_PLINKS,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshNbrOffsetMaxNeighbor,
				  1, 255, mask,
				  NL80211_MESHCONF_SYNC_OFFSET_MAX_NEIGHBOR,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPmaxPREQretries, 0, 255,
				  mask, NL80211_MESHCONF_HWMP_MAX_PREQ_RETRIES,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, path_refresh_time, 1, 65535,
				  mask, NL80211_MESHCONF_PATH_REFRESH_TIME,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, min_discovery_timeout, 1, 65535,
				  mask, NL80211_MESHCONF_MIN_DISCOVERY_TIMEOUT,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPactivePathTimeout,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_ACTIVE_PATH_TIMEOUT,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPpreqMinInterval,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_PREQ_MIN_INTERVAL,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPperrMinInterval,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_PERR_MIN_INTERVAL,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg,
				  dot11MeshHWMPnetDiameterTraversalTime,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_NET_DIAM_TRVS_TIME,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPRootMode, 0, 4,
				  mask, NL80211_MESHCONF_HWMP_ROOTMODE,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPRannInterval, 1, 65535,
				  mask, NL80211_MESHCONF_HWMP_RANN_INTERVAL,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg,
				  dot11MeshGateAnnouncementProtocol, 0, 1,
				  mask, NL80211_MESHCONF_GATE_ANNOUNCEMENTS,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshForwarding, 0, 1,
				  mask, NL80211_MESHCONF_FORWARDING,
				  nla_get_u8);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, rssi_threshold, 1, 255,
				  mask, NL80211_MESHCONF_RSSI_THRESHOLD,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, ht_opmode, 0, 16,
				  mask, NL80211_MESHCONF_HT_OPMODE,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMPactivePathToRootTimeout,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_PATH_TO_ROOT_TIMEOUT,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshHWMProotInterval, 1, 65535,
				  mask, NL80211_MESHCONF_HWMP_ROOT_INTERVAL,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg,
				  dot11MeshHWMPconfirmationInterval,
				  1, 65535, mask,
				  NL80211_MESHCONF_HWMP_CONFIRMATION_INTERVAL,
				  nla_get_u16);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, power_mode,
				  NL80211_MESH_POWER_ACTIVE,
				  NL80211_MESH_POWER_MAX,
				  mask, NL80211_MESHCONF_POWER_MODE,
				  nla_get_u32);
	FILL_IN_MESH_PARAM_IF_SET(tb, cfg, dot11MeshAwakeWindowDuration,
				  0, 65535, mask,
				  NL80211_MESHCONF_AWAKE_WINDOW, nla_get_u16);
	if (mask_out)
		*mask_out = mask;

	return 0;

#undef FILL_IN_MESH_PARAM_IF_SET
}