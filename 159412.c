int nl80211_parse_chandef(struct cfg80211_registered_device *rdev,
			  struct genl_info *info,
			  struct cfg80211_chan_def *chandef)
{
	struct netlink_ext_ack *extack = info->extack;
	struct nlattr **attrs = info->attrs;
	u32 control_freq;

	if (!attrs[NL80211_ATTR_WIPHY_FREQ])
		return -EINVAL;

	control_freq = nla_get_u32(attrs[NL80211_ATTR_WIPHY_FREQ]);

	memset(chandef, 0, sizeof(*chandef));

	chandef->chan = ieee80211_get_channel(&rdev->wiphy, control_freq);
	chandef->width = NL80211_CHAN_WIDTH_20_NOHT;
	chandef->center_freq1 = control_freq;
	chandef->center_freq2 = 0;

	/* Primary channel not allowed */
	if (!chandef->chan || chandef->chan->flags & IEEE80211_CHAN_DISABLED) {
		NL_SET_ERR_MSG_ATTR(extack, attrs[NL80211_ATTR_WIPHY_FREQ],
				    "Channel is disabled");
		return -EINVAL;
	}

	if (attrs[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
		enum nl80211_channel_type chantype;

		chantype = nla_get_u32(attrs[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);

		switch (chantype) {
		case NL80211_CHAN_NO_HT:
		case NL80211_CHAN_HT20:
		case NL80211_CHAN_HT40PLUS:
		case NL80211_CHAN_HT40MINUS:
			cfg80211_chandef_create(chandef, chandef->chan,
						chantype);
			/* user input for center_freq is incorrect */
			if (attrs[NL80211_ATTR_CENTER_FREQ1] &&
			    chandef->center_freq1 != nla_get_u32(attrs[NL80211_ATTR_CENTER_FREQ1])) {
				NL_SET_ERR_MSG_ATTR(extack,
						    attrs[NL80211_ATTR_CENTER_FREQ1],
						    "bad center frequency 1");
				return -EINVAL;
			}
			/* center_freq2 must be zero */
			if (attrs[NL80211_ATTR_CENTER_FREQ2] &&
			    nla_get_u32(attrs[NL80211_ATTR_CENTER_FREQ2])) {
				NL_SET_ERR_MSG_ATTR(extack,
						    attrs[NL80211_ATTR_CENTER_FREQ2],
						    "center frequency 2 can't be used");
				return -EINVAL;
			}
			break;
		default:
			NL_SET_ERR_MSG_ATTR(extack,
					    attrs[NL80211_ATTR_WIPHY_CHANNEL_TYPE],
					    "invalid channel type");
			return -EINVAL;
		}
	} else if (attrs[NL80211_ATTR_CHANNEL_WIDTH]) {
		chandef->width =
			nla_get_u32(attrs[NL80211_ATTR_CHANNEL_WIDTH]);
		if (attrs[NL80211_ATTR_CENTER_FREQ1])
			chandef->center_freq1 =
				nla_get_u32(attrs[NL80211_ATTR_CENTER_FREQ1]);
		if (attrs[NL80211_ATTR_CENTER_FREQ2])
			chandef->center_freq2 =
				nla_get_u32(attrs[NL80211_ATTR_CENTER_FREQ2]);
	}

	if (info->attrs[NL80211_ATTR_WIPHY_EDMG_CHANNELS]) {
		chandef->edmg.channels =
		      nla_get_u8(info->attrs[NL80211_ATTR_WIPHY_EDMG_CHANNELS]);

		if (info->attrs[NL80211_ATTR_WIPHY_EDMG_BW_CONFIG])
			chandef->edmg.bw_config =
		     nla_get_u8(info->attrs[NL80211_ATTR_WIPHY_EDMG_BW_CONFIG]);
	} else {
		chandef->edmg.bw_config = 0;
		chandef->edmg.channels = 0;
	}

	if (!cfg80211_chandef_valid(chandef)) {
		NL_SET_ERR_MSG(extack, "invalid channel definition");
		return -EINVAL;
	}

	if (!cfg80211_chandef_usable(&rdev->wiphy, chandef,
				     IEEE80211_CHAN_DISABLED)) {
		NL_SET_ERR_MSG(extack, "(extension) channel is disabled");
		return -EINVAL;
	}

	if ((chandef->width == NL80211_CHAN_WIDTH_5 ||
	     chandef->width == NL80211_CHAN_WIDTH_10) &&
	    !(rdev->wiphy.flags & WIPHY_FLAG_SUPPORTS_5_10_MHZ)) {
		NL_SET_ERR_MSG(extack, "5/10 MHz not supported");
		return -EINVAL;
	}

	return 0;
}