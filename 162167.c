struct sk_buff *cfg80211_testmode_alloc_event_skb(struct wiphy *wiphy,
						  int approxlen, gfp_t gfp)
{
	struct cfg80211_registered_device *rdev = wiphy_to_dev(wiphy);

	return __cfg80211_testmode_alloc_skb(rdev, approxlen, 0, 0, gfp);
}