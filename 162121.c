static int nl80211_send_mpath(struct sk_buff *msg, u32 portid, u32 seq,
				int flags, struct net_device *dev,
				u8 *dst, u8 *next_hop,
				struct mpath_info *pinfo)
{
	void *hdr;
	struct nlattr *pinfoattr;

	hdr = nl80211hdr_put(msg, portid, seq, flags, NL80211_CMD_NEW_STATION);
	if (!hdr)
		return -1;

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, dst) ||
	    nla_put(msg, NL80211_ATTR_MPATH_NEXT_HOP, ETH_ALEN, next_hop) ||
	    nla_put_u32(msg, NL80211_ATTR_GENERATION, pinfo->generation))
		goto nla_put_failure;

	pinfoattr = nla_nest_start(msg, NL80211_ATTR_MPATH_INFO);
	if (!pinfoattr)
		goto nla_put_failure;
	if ((pinfo->filled & MPATH_INFO_FRAME_QLEN) &&
	    nla_put_u32(msg, NL80211_MPATH_INFO_FRAME_QLEN,
			pinfo->frame_qlen))
		goto nla_put_failure;
	if (((pinfo->filled & MPATH_INFO_SN) &&
	     nla_put_u32(msg, NL80211_MPATH_INFO_SN, pinfo->sn)) ||
	    ((pinfo->filled & MPATH_INFO_METRIC) &&
	     nla_put_u32(msg, NL80211_MPATH_INFO_METRIC,
			 pinfo->metric)) ||
	    ((pinfo->filled & MPATH_INFO_EXPTIME) &&
	     nla_put_u32(msg, NL80211_MPATH_INFO_EXPTIME,
			 pinfo->exptime)) ||
	    ((pinfo->filled & MPATH_INFO_FLAGS) &&
	     nla_put_u8(msg, NL80211_MPATH_INFO_FLAGS,
			pinfo->flags)) ||
	    ((pinfo->filled & MPATH_INFO_DISCOVERY_TIMEOUT) &&
	     nla_put_u32(msg, NL80211_MPATH_INFO_DISCOVERY_TIMEOUT,
			 pinfo->discovery_timeout)) ||
	    ((pinfo->filled & MPATH_INFO_DISCOVERY_RETRIES) &&
	     nla_put_u8(msg, NL80211_MPATH_INFO_DISCOVERY_RETRIES,
			pinfo->discovery_retries)))
		goto nla_put_failure;

	nla_nest_end(msg, pinfoattr);

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}