struct sk_buff *cfg80211_testmode_alloc_reply_skb(struct wiphy *wiphy,
						  int approxlen)
{
	struct cfg80211_registered_device *rdev = wiphy_to_dev(wiphy);

	if (WARN_ON(!rdev->testmode_info))
		return NULL;

	return __cfg80211_testmode_alloc_skb(rdev, approxlen,
				rdev->testmode_info->snd_portid,
				rdev->testmode_info->snd_seq,
				GFP_KERNEL);
}