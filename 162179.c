void nl80211_send_remain_on_channel_cancel(
	struct cfg80211_registered_device *rdev,
	struct wireless_dev *wdev,
	u64 cookie, struct ieee80211_channel *chan, gfp_t gfp)
{
	nl80211_send_remain_on_chan_event(NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL,
					  rdev, wdev, cookie, chan, 0, gfp);
}