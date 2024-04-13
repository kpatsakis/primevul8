static int nl80211_join_mesh(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct mesh_config cfg;
	struct mesh_setup setup;
	int err;

	/* start with default */
	memcpy(&cfg, &default_mesh_config, sizeof(cfg));
	memcpy(&setup, &default_mesh_setup, sizeof(setup));

	if (info->attrs[NL80211_ATTR_MESH_CONFIG]) {
		/* and parse parameters if given */
		err = nl80211_parse_mesh_config(info, &cfg, NULL);
		if (err)
			return err;
	}

	if (!info->attrs[NL80211_ATTR_MESH_ID] ||
	    !nla_len(info->attrs[NL80211_ATTR_MESH_ID]))
		return -EINVAL;

	setup.mesh_id = nla_data(info->attrs[NL80211_ATTR_MESH_ID]);
	setup.mesh_id_len = nla_len(info->attrs[NL80211_ATTR_MESH_ID]);

	if (info->attrs[NL80211_ATTR_MCAST_RATE] &&
	    !nl80211_parse_mcast_rate(rdev, setup.mcast_rate,
			    nla_get_u32(info->attrs[NL80211_ATTR_MCAST_RATE])))
			return -EINVAL;

	if (info->attrs[NL80211_ATTR_BEACON_INTERVAL]) {
		setup.beacon_interval =
			nla_get_u32(info->attrs[NL80211_ATTR_BEACON_INTERVAL]);
		if (setup.beacon_interval < 10 ||
		    setup.beacon_interval > 10000)
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_DTIM_PERIOD]) {
		setup.dtim_period =
			nla_get_u32(info->attrs[NL80211_ATTR_DTIM_PERIOD]);
		if (setup.dtim_period < 1 || setup.dtim_period > 100)
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_MESH_SETUP]) {
		/* parse additional setup parameters if given */
		err = nl80211_parse_mesh_setup(info, &setup);
		if (err)
			return err;
	}

	if (info->attrs[NL80211_ATTR_WIPHY_FREQ]) {
		err = nl80211_parse_chandef(rdev, info, &setup.chandef);
		if (err)
			return err;
	} else {
		/* cfg80211_join_mesh() will sort it out */
		setup.chandef.chan = NULL;
	}

	return cfg80211_join_mesh(rdev, dev, &setup, &cfg);
}