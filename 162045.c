static int nl80211_parse_chandef(struct cfg80211_registered_device *rdev,
				 struct genl_info *info,
				 struct cfg80211_chan_def *chandef)
{
	u32 control_freq;

	if (!info->attrs[NL80211_ATTR_WIPHY_FREQ])
		return -EINVAL;

	control_freq = nla_get_u32(info->attrs[NL80211_ATTR_WIPHY_FREQ]);

	chandef->chan = ieee80211_get_channel(&rdev->wiphy, control_freq);
	chandef->width = NL80211_CHAN_WIDTH_20_NOHT;
	chandef->center_freq1 = control_freq;
	chandef->center_freq2 = 0;

	/* Primary channel not allowed */
	if (!chandef->chan || chandef->chan->flags & IEEE80211_CHAN_DISABLED)
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
		enum nl80211_channel_type chantype;

		chantype = nla_get_u32(
				info->attrs[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);

		switch (chantype) {
		case NL80211_CHAN_NO_HT:
		case NL80211_CHAN_HT20:
		case NL80211_CHAN_HT40PLUS:
		case NL80211_CHAN_HT40MINUS:
			cfg80211_chandef_create(chandef, chandef->chan,
						chantype);
			break;
		default:
			return -EINVAL;
		}
	} else if (info->attrs[NL80211_ATTR_CHANNEL_WIDTH]) {
		chandef->width =
			nla_get_u32(info->attrs[NL80211_ATTR_CHANNEL_WIDTH]);
		if (info->attrs[NL80211_ATTR_CENTER_FREQ1])
			chandef->center_freq1 =
				nla_get_u32(
					info->attrs[NL80211_ATTR_CENTER_FREQ1]);
		if (info->attrs[NL80211_ATTR_CENTER_FREQ2])
			chandef->center_freq2 =
				nla_get_u32(
					info->attrs[NL80211_ATTR_CENTER_FREQ2]);
	}

	if (!cfg80211_chandef_valid(chandef))
		return -EINVAL;

	if (!cfg80211_chandef_usable(&rdev->wiphy, chandef,
				     IEEE80211_CHAN_DISABLED))
		return -EINVAL;

	return 0;
}