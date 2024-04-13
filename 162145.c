static int nl80211_send_bss(struct sk_buff *msg, struct netlink_callback *cb,
			    u32 seq, int flags,
			    struct cfg80211_registered_device *rdev,
			    struct wireless_dev *wdev,
			    struct cfg80211_internal_bss *intbss)
{
	struct cfg80211_bss *res = &intbss->pub;
	const struct cfg80211_bss_ies *ies;
	void *hdr;
	struct nlattr *bss;

	ASSERT_WDEV_LOCK(wdev);

	hdr = nl80211hdr_put(msg, NETLINK_CB(cb->skb).portid, seq, flags,
			     NL80211_CMD_NEW_SCAN_RESULTS);
	if (!hdr)
		return -1;

	genl_dump_check_consistent(cb, hdr, &nl80211_fam);

	if (nla_put_u32(msg, NL80211_ATTR_GENERATION, rdev->bss_generation) ||
	    nla_put_u32(msg, NL80211_ATTR_IFINDEX, wdev->netdev->ifindex))
		goto nla_put_failure;

	bss = nla_nest_start(msg, NL80211_ATTR_BSS);
	if (!bss)
		goto nla_put_failure;
	if ((!is_zero_ether_addr(res->bssid) &&
	     nla_put(msg, NL80211_BSS_BSSID, ETH_ALEN, res->bssid)))
		goto nla_put_failure;

	rcu_read_lock();
	ies = rcu_dereference(res->ies);
	if (ies && ies->len && nla_put(msg, NL80211_BSS_INFORMATION_ELEMENTS,
				       ies->len, ies->data)) {
		rcu_read_unlock();
		goto nla_put_failure;
	}
	ies = rcu_dereference(res->beacon_ies);
	if (ies && ies->len && nla_put(msg, NL80211_BSS_BEACON_IES,
				       ies->len, ies->data)) {
		rcu_read_unlock();
		goto nla_put_failure;
	}
	rcu_read_unlock();

	if (res->tsf &&
	    nla_put_u64(msg, NL80211_BSS_TSF, res->tsf))
		goto nla_put_failure;
	if (res->beacon_interval &&
	    nla_put_u16(msg, NL80211_BSS_BEACON_INTERVAL, res->beacon_interval))
		goto nla_put_failure;
	if (nla_put_u16(msg, NL80211_BSS_CAPABILITY, res->capability) ||
	    nla_put_u32(msg, NL80211_BSS_FREQUENCY, res->channel->center_freq) ||
	    nla_put_u32(msg, NL80211_BSS_SEEN_MS_AGO,
			jiffies_to_msecs(jiffies - intbss->ts)))
		goto nla_put_failure;

	switch (rdev->wiphy.signal_type) {
	case CFG80211_SIGNAL_TYPE_MBM:
		if (nla_put_u32(msg, NL80211_BSS_SIGNAL_MBM, res->signal))
			goto nla_put_failure;
		break;
	case CFG80211_SIGNAL_TYPE_UNSPEC:
		if (nla_put_u8(msg, NL80211_BSS_SIGNAL_UNSPEC, res->signal))
			goto nla_put_failure;
		break;
	default:
		break;
	}

	switch (wdev->iftype) {
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_STATION:
		if (intbss == wdev->current_bss &&
		    nla_put_u32(msg, NL80211_BSS_STATUS,
				NL80211_BSS_STATUS_ASSOCIATED))
			goto nla_put_failure;
		break;
	case NL80211_IFTYPE_ADHOC:
		if (intbss == wdev->current_bss &&
		    nla_put_u32(msg, NL80211_BSS_STATUS,
				NL80211_BSS_STATUS_IBSS_JOINED))
			goto nla_put_failure;
		break;
	default:
		break;
	}

	nla_nest_end(msg, bss);

	return genlmsg_end(msg, hdr);

 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}