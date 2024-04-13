static int nl80211_send_chandef(struct sk_buff *msg,
				 struct cfg80211_chan_def *chandef)
{
	WARN_ON(!cfg80211_chandef_valid(chandef));

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY_FREQ,
			chandef->chan->center_freq))
		return -ENOBUFS;
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
	case NL80211_CHAN_WIDTH_20:
	case NL80211_CHAN_WIDTH_40:
		if (nla_put_u32(msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE,
				cfg80211_get_chandef_type(chandef)))
			return -ENOBUFS;
		break;
	default:
		break;
	}
	if (nla_put_u32(msg, NL80211_ATTR_CHANNEL_WIDTH, chandef->width))
		return -ENOBUFS;
	if (nla_put_u32(msg, NL80211_ATTR_CENTER_FREQ1, chandef->center_freq1))
		return -ENOBUFS;
	if (chandef->center_freq2 &&
	    nla_put_u32(msg, NL80211_ATTR_CENTER_FREQ2, chandef->center_freq2))
		return -ENOBUFS;
	return 0;
}