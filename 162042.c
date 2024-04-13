bool nl80211_unexpected_4addr_frame(struct net_device *dev,
				    const u8 *addr, gfp_t gfp)
{
	return __nl80211_unexpected_frame(dev,
					  NL80211_CMD_UNEXPECTED_4ADDR_FRAME,
					  addr, gfp);
}