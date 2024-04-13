void nl80211_send_disassoc(struct cfg80211_registered_device *rdev,
			   struct net_device *netdev, const u8 *buf,
			   size_t len, gfp_t gfp)
{
	nl80211_send_mlme_event(rdev, netdev, buf, len,
				NL80211_CMD_DISASSOCIATE, gfp);
}