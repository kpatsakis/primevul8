static int nl80211_send_wiphy(struct sk_buff *msg, u32 portid, u32 seq, int flags,
			      struct cfg80211_registered_device *dev)
{
	void *hdr;
	struct nlattr *nl_bands, *nl_band;
	struct nlattr *nl_freqs, *nl_freq;
	struct nlattr *nl_rates, *nl_rate;
	struct nlattr *nl_cmds;
	enum ieee80211_band band;
	struct ieee80211_channel *chan;
	struct ieee80211_rate *rate;
	int i;
	const struct ieee80211_txrx_stypes *mgmt_stypes =
				dev->wiphy.mgmt_stypes;

	hdr = nl80211hdr_put(msg, portid, seq, flags, NL80211_CMD_NEW_WIPHY);
	if (!hdr)
		return -1;

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, dev->wiphy_idx) ||
	    nla_put_string(msg, NL80211_ATTR_WIPHY_NAME, wiphy_name(&dev->wiphy)) ||
	    nla_put_u32(msg, NL80211_ATTR_GENERATION,
			cfg80211_rdev_list_generation) ||
	    nla_put_u8(msg, NL80211_ATTR_WIPHY_RETRY_SHORT,
		       dev->wiphy.retry_short) ||
	    nla_put_u8(msg, NL80211_ATTR_WIPHY_RETRY_LONG,
		       dev->wiphy.retry_long) ||
	    nla_put_u32(msg, NL80211_ATTR_WIPHY_FRAG_THRESHOLD,
			dev->wiphy.frag_threshold) ||
	    nla_put_u32(msg, NL80211_ATTR_WIPHY_RTS_THRESHOLD,
			dev->wiphy.rts_threshold) ||
	    nla_put_u8(msg, NL80211_ATTR_WIPHY_COVERAGE_CLASS,
		       dev->wiphy.coverage_class) ||
	    nla_put_u8(msg, NL80211_ATTR_MAX_NUM_SCAN_SSIDS,
		       dev->wiphy.max_scan_ssids) ||
	    nla_put_u8(msg, NL80211_ATTR_MAX_NUM_SCHED_SCAN_SSIDS,
		       dev->wiphy.max_sched_scan_ssids) ||
	    nla_put_u16(msg, NL80211_ATTR_MAX_SCAN_IE_LEN,
			dev->wiphy.max_scan_ie_len) ||
	    nla_put_u16(msg, NL80211_ATTR_MAX_SCHED_SCAN_IE_LEN,
			dev->wiphy.max_sched_scan_ie_len) ||
	    nla_put_u8(msg, NL80211_ATTR_MAX_MATCH_SETS,
		       dev->wiphy.max_match_sets))
		goto nla_put_failure;

	if ((dev->wiphy.flags & WIPHY_FLAG_IBSS_RSN) &&
	    nla_put_flag(msg, NL80211_ATTR_SUPPORT_IBSS_RSN))
		goto nla_put_failure;
	if ((dev->wiphy.flags & WIPHY_FLAG_MESH_AUTH) &&
	    nla_put_flag(msg, NL80211_ATTR_SUPPORT_MESH_AUTH))
		goto nla_put_failure;
	if ((dev->wiphy.flags & WIPHY_FLAG_AP_UAPSD) &&
	    nla_put_flag(msg, NL80211_ATTR_SUPPORT_AP_UAPSD))
		goto nla_put_failure;
	if ((dev->wiphy.flags & WIPHY_FLAG_SUPPORTS_FW_ROAM) &&
	    nla_put_flag(msg, NL80211_ATTR_ROAM_SUPPORT))
		goto nla_put_failure;
	if ((dev->wiphy.flags & WIPHY_FLAG_SUPPORTS_TDLS) &&
	    nla_put_flag(msg, NL80211_ATTR_TDLS_SUPPORT))
		goto nla_put_failure;
	if ((dev->wiphy.flags & WIPHY_FLAG_TDLS_EXTERNAL_SETUP) &&
	    nla_put_flag(msg, NL80211_ATTR_TDLS_EXTERNAL_SETUP))
		goto nla_put_failure;

	if (nla_put(msg, NL80211_ATTR_CIPHER_SUITES,
		    sizeof(u32) * dev->wiphy.n_cipher_suites,
		    dev->wiphy.cipher_suites))
		goto nla_put_failure;

	if (nla_put_u8(msg, NL80211_ATTR_MAX_NUM_PMKIDS,
		       dev->wiphy.max_num_pmkids))
		goto nla_put_failure;

	if ((dev->wiphy.flags & WIPHY_FLAG_CONTROL_PORT_PROTOCOL) &&
	    nla_put_flag(msg, NL80211_ATTR_CONTROL_PORT_ETHERTYPE))
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY_ANTENNA_AVAIL_TX,
			dev->wiphy.available_antennas_tx) ||
	    nla_put_u32(msg, NL80211_ATTR_WIPHY_ANTENNA_AVAIL_RX,
			dev->wiphy.available_antennas_rx))
		goto nla_put_failure;

	if ((dev->wiphy.flags & WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD) &&
	    nla_put_u32(msg, NL80211_ATTR_PROBE_RESP_OFFLOAD,
			dev->wiphy.probe_resp_offload))
		goto nla_put_failure;

	if ((dev->wiphy.available_antennas_tx ||
	     dev->wiphy.available_antennas_rx) && dev->ops->get_antenna) {
		u32 tx_ant = 0, rx_ant = 0;
		int res;
		res = rdev_get_antenna(dev, &tx_ant, &rx_ant);
		if (!res) {
			if (nla_put_u32(msg, NL80211_ATTR_WIPHY_ANTENNA_TX,
					tx_ant) ||
			    nla_put_u32(msg, NL80211_ATTR_WIPHY_ANTENNA_RX,
					rx_ant))
				goto nla_put_failure;
		}
	}

	if (nl80211_put_iftypes(msg, NL80211_ATTR_SUPPORTED_IFTYPES,
				dev->wiphy.interface_modes))
		goto nla_put_failure;

	nl_bands = nla_nest_start(msg, NL80211_ATTR_WIPHY_BANDS);
	if (!nl_bands)
		goto nla_put_failure;

	for (band = 0; band < IEEE80211_NUM_BANDS; band++) {
		if (!dev->wiphy.bands[band])
			continue;

		nl_band = nla_nest_start(msg, band);
		if (!nl_band)
			goto nla_put_failure;

		/* add HT info */
		if (dev->wiphy.bands[band]->ht_cap.ht_supported &&
		    (nla_put(msg, NL80211_BAND_ATTR_HT_MCS_SET,
			     sizeof(dev->wiphy.bands[band]->ht_cap.mcs),
			     &dev->wiphy.bands[band]->ht_cap.mcs) ||
		     nla_put_u16(msg, NL80211_BAND_ATTR_HT_CAPA,
				 dev->wiphy.bands[band]->ht_cap.cap) ||
		     nla_put_u8(msg, NL80211_BAND_ATTR_HT_AMPDU_FACTOR,
				dev->wiphy.bands[band]->ht_cap.ampdu_factor) ||
		     nla_put_u8(msg, NL80211_BAND_ATTR_HT_AMPDU_DENSITY,
				dev->wiphy.bands[band]->ht_cap.ampdu_density)))
			goto nla_put_failure;

		/* add VHT info */
		if (dev->wiphy.bands[band]->vht_cap.vht_supported &&
		    (nla_put(msg, NL80211_BAND_ATTR_VHT_MCS_SET,
			     sizeof(dev->wiphy.bands[band]->vht_cap.vht_mcs),
			     &dev->wiphy.bands[band]->vht_cap.vht_mcs) ||
		     nla_put_u32(msg, NL80211_BAND_ATTR_VHT_CAPA,
				 dev->wiphy.bands[band]->vht_cap.cap)))
			goto nla_put_failure;

		/* add frequencies */
		nl_freqs = nla_nest_start(msg, NL80211_BAND_ATTR_FREQS);
		if (!nl_freqs)
			goto nla_put_failure;

		for (i = 0; i < dev->wiphy.bands[band]->n_channels; i++) {
			nl_freq = nla_nest_start(msg, i);
			if (!nl_freq)
				goto nla_put_failure;

			chan = &dev->wiphy.bands[band]->channels[i];

			if (nl80211_msg_put_channel(msg, chan))
				goto nla_put_failure;

			nla_nest_end(msg, nl_freq);
		}

		nla_nest_end(msg, nl_freqs);

		/* add bitrates */
		nl_rates = nla_nest_start(msg, NL80211_BAND_ATTR_RATES);
		if (!nl_rates)
			goto nla_put_failure;

		for (i = 0; i < dev->wiphy.bands[band]->n_bitrates; i++) {
			nl_rate = nla_nest_start(msg, i);
			if (!nl_rate)
				goto nla_put_failure;

			rate = &dev->wiphy.bands[band]->bitrates[i];
			if (nla_put_u32(msg, NL80211_BITRATE_ATTR_RATE,
					rate->bitrate))
				goto nla_put_failure;
			if ((rate->flags & IEEE80211_RATE_SHORT_PREAMBLE) &&
			    nla_put_flag(msg,
					 NL80211_BITRATE_ATTR_2GHZ_SHORTPREAMBLE))
				goto nla_put_failure;

			nla_nest_end(msg, nl_rate);
		}

		nla_nest_end(msg, nl_rates);

		nla_nest_end(msg, nl_band);
	}
	nla_nest_end(msg, nl_bands);

	nl_cmds = nla_nest_start(msg, NL80211_ATTR_SUPPORTED_COMMANDS);
	if (!nl_cmds)
		goto nla_put_failure;

	i = 0;
#define CMD(op, n)						\
	 do {							\
		if (dev->ops->op) {				\
			i++;					\
			if (nla_put_u32(msg, i, NL80211_CMD_ ## n)) \
				goto nla_put_failure;		\
		}						\
	} while (0)

	CMD(add_virtual_intf, NEW_INTERFACE);
	CMD(change_virtual_intf, SET_INTERFACE);
	CMD(add_key, NEW_KEY);
	CMD(start_ap, START_AP);
	CMD(add_station, NEW_STATION);
	CMD(add_mpath, NEW_MPATH);
	CMD(update_mesh_config, SET_MESH_CONFIG);
	CMD(change_bss, SET_BSS);
	CMD(auth, AUTHENTICATE);
	CMD(assoc, ASSOCIATE);
	CMD(deauth, DEAUTHENTICATE);
	CMD(disassoc, DISASSOCIATE);
	CMD(join_ibss, JOIN_IBSS);
	CMD(join_mesh, JOIN_MESH);
	CMD(set_pmksa, SET_PMKSA);
	CMD(del_pmksa, DEL_PMKSA);
	CMD(flush_pmksa, FLUSH_PMKSA);
	if (dev->wiphy.flags & WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL)
		CMD(remain_on_channel, REMAIN_ON_CHANNEL);
	CMD(set_bitrate_mask, SET_TX_BITRATE_MASK);
	CMD(mgmt_tx, FRAME);
	CMD(mgmt_tx_cancel_wait, FRAME_WAIT_CANCEL);
	if (dev->wiphy.flags & WIPHY_FLAG_NETNS_OK) {
		i++;
		if (nla_put_u32(msg, i, NL80211_CMD_SET_WIPHY_NETNS))
			goto nla_put_failure;
	}
	if (dev->ops->set_monitor_channel || dev->ops->start_ap ||
	    dev->ops->join_mesh) {
		i++;
		if (nla_put_u32(msg, i, NL80211_CMD_SET_CHANNEL))
			goto nla_put_failure;
	}
	CMD(set_wds_peer, SET_WDS_PEER);
	if (dev->wiphy.flags & WIPHY_FLAG_SUPPORTS_TDLS) {
		CMD(tdls_mgmt, TDLS_MGMT);
		CMD(tdls_oper, TDLS_OPER);
	}
	if (dev->wiphy.flags & WIPHY_FLAG_SUPPORTS_SCHED_SCAN)
		CMD(sched_scan_start, START_SCHED_SCAN);
	CMD(probe_client, PROBE_CLIENT);
	CMD(set_noack_map, SET_NOACK_MAP);
	if (dev->wiphy.flags & WIPHY_FLAG_REPORTS_OBSS) {
		i++;
		if (nla_put_u32(msg, i, NL80211_CMD_REGISTER_BEACONS))
			goto nla_put_failure;
	}
	CMD(start_p2p_device, START_P2P_DEVICE);
	CMD(set_mcast_rate, SET_MCAST_RATE);

#ifdef CONFIG_NL80211_TESTMODE
	CMD(testmode_cmd, TESTMODE);
#endif

#undef CMD

	if (dev->ops->connect || dev->ops->auth) {
		i++;
		if (nla_put_u32(msg, i, NL80211_CMD_CONNECT))
			goto nla_put_failure;
	}

	if (dev->ops->disconnect || dev->ops->deauth) {
		i++;
		if (nla_put_u32(msg, i, NL80211_CMD_DISCONNECT))
			goto nla_put_failure;
	}

	nla_nest_end(msg, nl_cmds);

	if (dev->ops->remain_on_channel &&
	    (dev->wiphy.flags & WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL) &&
	    nla_put_u32(msg, NL80211_ATTR_MAX_REMAIN_ON_CHANNEL_DURATION,
			dev->wiphy.max_remain_on_channel_duration))
		goto nla_put_failure;

	if ((dev->wiphy.flags & WIPHY_FLAG_OFFCHAN_TX) &&
	    nla_put_flag(msg, NL80211_ATTR_OFFCHANNEL_TX_OK))
		goto nla_put_failure;

	if (mgmt_stypes) {
		u16 stypes;
		struct nlattr *nl_ftypes, *nl_ifs;
		enum nl80211_iftype ift;

		nl_ifs = nla_nest_start(msg, NL80211_ATTR_TX_FRAME_TYPES);
		if (!nl_ifs)
			goto nla_put_failure;

		for (ift = 0; ift < NUM_NL80211_IFTYPES; ift++) {
			nl_ftypes = nla_nest_start(msg, ift);
			if (!nl_ftypes)
				goto nla_put_failure;
			i = 0;
			stypes = mgmt_stypes[ift].tx;
			while (stypes) {
				if ((stypes & 1) &&
				    nla_put_u16(msg, NL80211_ATTR_FRAME_TYPE,
						(i << 4) | IEEE80211_FTYPE_MGMT))
					goto nla_put_failure;
				stypes >>= 1;
				i++;
			}
			nla_nest_end(msg, nl_ftypes);
		}

		nla_nest_end(msg, nl_ifs);

		nl_ifs = nla_nest_start(msg, NL80211_ATTR_RX_FRAME_TYPES);
		if (!nl_ifs)
			goto nla_put_failure;

		for (ift = 0; ift < NUM_NL80211_IFTYPES; ift++) {
			nl_ftypes = nla_nest_start(msg, ift);
			if (!nl_ftypes)
				goto nla_put_failure;
			i = 0;
			stypes = mgmt_stypes[ift].rx;
			while (stypes) {
				if ((stypes & 1) &&
				    nla_put_u16(msg, NL80211_ATTR_FRAME_TYPE,
						(i << 4) | IEEE80211_FTYPE_MGMT))
					goto nla_put_failure;
				stypes >>= 1;
				i++;
			}
			nla_nest_end(msg, nl_ftypes);
		}
		nla_nest_end(msg, nl_ifs);
	}

#ifdef CONFIG_PM
	if (dev->wiphy.wowlan.flags || dev->wiphy.wowlan.n_patterns) {
		struct nlattr *nl_wowlan;

		nl_wowlan = nla_nest_start(msg,
				NL80211_ATTR_WOWLAN_TRIGGERS_SUPPORTED);
		if (!nl_wowlan)
			goto nla_put_failure;

		if (((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_ANY) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_ANY)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_DISCONNECT) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_DISCONNECT)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_MAGIC_PKT) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_MAGIC_PKT)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_SUPPORTS_GTK_REKEY) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_GTK_REKEY_SUPPORTED)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_GTK_REKEY_FAILURE) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_GTK_REKEY_FAILURE)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_EAP_IDENTITY_REQ) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_EAP_IDENT_REQUEST)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_4WAY_HANDSHAKE) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_4WAY_HANDSHAKE)) ||
		    ((dev->wiphy.wowlan.flags & WIPHY_WOWLAN_RFKILL_RELEASE) &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_RFKILL_RELEASE)))
		    goto nla_put_failure;
		if (dev->wiphy.wowlan.n_patterns) {
			struct nl80211_wowlan_pattern_support pat = {
				.max_patterns = dev->wiphy.wowlan.n_patterns,
				.min_pattern_len =
					dev->wiphy.wowlan.pattern_min_len,
				.max_pattern_len =
					dev->wiphy.wowlan.pattern_max_len,
			};
			if (nla_put(msg, NL80211_WOWLAN_TRIG_PKT_PATTERN,
				    sizeof(pat), &pat))
				goto nla_put_failure;
		}

		nla_nest_end(msg, nl_wowlan);
	}
#endif

	if (nl80211_put_iftypes(msg, NL80211_ATTR_SOFTWARE_IFTYPES,
				dev->wiphy.software_iftypes))
		goto nla_put_failure;

	if (nl80211_put_iface_combinations(&dev->wiphy, msg))
		goto nla_put_failure;

	if ((dev->wiphy.flags & WIPHY_FLAG_HAVE_AP_SME) &&
	    nla_put_u32(msg, NL80211_ATTR_DEVICE_AP_SME,
			dev->wiphy.ap_sme_capa))
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_ATTR_FEATURE_FLAGS,
			dev->wiphy.features))
		goto nla_put_failure;

	if (dev->wiphy.ht_capa_mod_mask &&
	    nla_put(msg, NL80211_ATTR_HT_CAPABILITY_MASK,
		    sizeof(*dev->wiphy.ht_capa_mod_mask),
		    dev->wiphy.ht_capa_mod_mask))
		goto nla_put_failure;

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}