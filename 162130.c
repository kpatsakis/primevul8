static int nl80211_msg_put_channel(struct sk_buff *msg,
				   struct ieee80211_channel *chan)
{
	if (nla_put_u32(msg, NL80211_FREQUENCY_ATTR_FREQ,
			chan->center_freq))
		goto nla_put_failure;

	if ((chan->flags & IEEE80211_CHAN_DISABLED) &&
	    nla_put_flag(msg, NL80211_FREQUENCY_ATTR_DISABLED))
		goto nla_put_failure;
	if ((chan->flags & IEEE80211_CHAN_PASSIVE_SCAN) &&
	    nla_put_flag(msg, NL80211_FREQUENCY_ATTR_PASSIVE_SCAN))
		goto nla_put_failure;
	if ((chan->flags & IEEE80211_CHAN_NO_IBSS) &&
	    nla_put_flag(msg, NL80211_FREQUENCY_ATTR_NO_IBSS))
		goto nla_put_failure;
	if ((chan->flags & IEEE80211_CHAN_RADAR) &&
	    nla_put_flag(msg, NL80211_FREQUENCY_ATTR_RADAR))
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_FREQUENCY_ATTR_MAX_TX_POWER,
			DBM_TO_MBM(chan->max_power)))
		goto nla_put_failure;

	return 0;

 nla_put_failure:
	return -ENOBUFS;
}