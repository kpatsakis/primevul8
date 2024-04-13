static bool nl80211_put_sta_rate(struct sk_buff *msg, struct rate_info *info,
				 int attr)
{
	struct nlattr *rate;
	u32 bitrate;
	u16 bitrate_compat;

	rate = nla_nest_start(msg, attr);
	if (!rate)
		return false;

	/* cfg80211_calculate_bitrate will return 0 for mcs >= 32 */
	bitrate = cfg80211_calculate_bitrate(info);
	/* report 16-bit bitrate only if we can */
	bitrate_compat = bitrate < (1UL << 16) ? bitrate : 0;
	if (bitrate > 0 &&
	    nla_put_u32(msg, NL80211_RATE_INFO_BITRATE32, bitrate))
		return false;
	if (bitrate_compat > 0 &&
	    nla_put_u16(msg, NL80211_RATE_INFO_BITRATE, bitrate_compat))
		return false;

	if (info->flags & RATE_INFO_FLAGS_MCS) {
		if (nla_put_u8(msg, NL80211_RATE_INFO_MCS, info->mcs))
			return false;
		if (info->flags & RATE_INFO_FLAGS_40_MHZ_WIDTH &&
		    nla_put_flag(msg, NL80211_RATE_INFO_40_MHZ_WIDTH))
			return false;
		if (info->flags & RATE_INFO_FLAGS_SHORT_GI &&
		    nla_put_flag(msg, NL80211_RATE_INFO_SHORT_GI))
			return false;
	} else if (info->flags & RATE_INFO_FLAGS_VHT_MCS) {
		if (nla_put_u8(msg, NL80211_RATE_INFO_VHT_MCS, info->mcs))
			return false;
		if (nla_put_u8(msg, NL80211_RATE_INFO_VHT_NSS, info->nss))
			return false;
		if (info->flags & RATE_INFO_FLAGS_40_MHZ_WIDTH &&
		    nla_put_flag(msg, NL80211_RATE_INFO_40_MHZ_WIDTH))
			return false;
		if (info->flags & RATE_INFO_FLAGS_80_MHZ_WIDTH &&
		    nla_put_flag(msg, NL80211_RATE_INFO_80_MHZ_WIDTH))
			return false;
		if (info->flags & RATE_INFO_FLAGS_80P80_MHZ_WIDTH &&
		    nla_put_flag(msg, NL80211_RATE_INFO_80P80_MHZ_WIDTH))
			return false;
		if (info->flags & RATE_INFO_FLAGS_160_MHZ_WIDTH &&
		    nla_put_flag(msg, NL80211_RATE_INFO_160_MHZ_WIDTH))
			return false;
		if (info->flags & RATE_INFO_FLAGS_SHORT_GI &&
		    nla_put_flag(msg, NL80211_RATE_INFO_SHORT_GI))
			return false;
	}

	nla_nest_end(msg, rate);
	return true;
}