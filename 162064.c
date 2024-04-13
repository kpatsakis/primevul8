static int nl80211_send_survey(struct sk_buff *msg, u32 portid, u32 seq,
				int flags, struct net_device *dev,
				struct survey_info *survey)
{
	void *hdr;
	struct nlattr *infoattr;

	hdr = nl80211hdr_put(msg, portid, seq, flags,
			     NL80211_CMD_NEW_SURVEY_RESULTS);
	if (!hdr)
		return -ENOMEM;

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex))
		goto nla_put_failure;

	infoattr = nla_nest_start(msg, NL80211_ATTR_SURVEY_INFO);
	if (!infoattr)
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_SURVEY_INFO_FREQUENCY,
			survey->channel->center_freq))
		goto nla_put_failure;

	if ((survey->filled & SURVEY_INFO_NOISE_DBM) &&
	    nla_put_u8(msg, NL80211_SURVEY_INFO_NOISE, survey->noise))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_IN_USE) &&
	    nla_put_flag(msg, NL80211_SURVEY_INFO_IN_USE))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_CHANNEL_TIME) &&
	    nla_put_u64(msg, NL80211_SURVEY_INFO_CHANNEL_TIME,
			survey->channel_time))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_CHANNEL_TIME_BUSY) &&
	    nla_put_u64(msg, NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY,
			survey->channel_time_busy))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_CHANNEL_TIME_EXT_BUSY) &&
	    nla_put_u64(msg, NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY,
			survey->channel_time_ext_busy))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_CHANNEL_TIME_RX) &&
	    nla_put_u64(msg, NL80211_SURVEY_INFO_CHANNEL_TIME_RX,
			survey->channel_time_rx))
		goto nla_put_failure;
	if ((survey->filled & SURVEY_INFO_CHANNEL_TIME_TX) &&
	    nla_put_u64(msg, NL80211_SURVEY_INFO_CHANNEL_TIME_TX,
			survey->channel_time_tx))
		goto nla_put_failure;

	nla_nest_end(msg, infoattr);

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}