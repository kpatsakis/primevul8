static int nl80211_send_iface(struct sk_buff *msg, u32 portid, u32 seq, int flags,
			      struct cfg80211_registered_device *rdev,
			      struct wireless_dev *wdev,
			      enum nl80211_commands cmd)
{
	struct net_device *dev = wdev->netdev;
	void *hdr;

	WARN_ON(cmd != NL80211_CMD_NEW_INTERFACE &&
		cmd != NL80211_CMD_DEL_INTERFACE &&
		cmd != NL80211_CMD_SET_INTERFACE);

	hdr = nl80211hdr_put(msg, portid, seq, flags, cmd);
	if (!hdr)
		return -1;

	if (dev &&
	    (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	     nla_put_string(msg, NL80211_ATTR_IFNAME, dev->name)))
		goto nla_put_failure;

	if (nla_put_u32(msg, NL80211_ATTR_WIPHY, rdev->wiphy_idx) ||
	    nla_put_u32(msg, NL80211_ATTR_IFTYPE, wdev->iftype) ||
	    nla_put_u64_64bit(msg, NL80211_ATTR_WDEV, wdev_id(wdev),
			      NL80211_ATTR_PAD) ||
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, wdev_address(wdev)) ||
	    nla_put_u32(msg, NL80211_ATTR_GENERATION,
			rdev->devlist_generation ^
			(cfg80211_rdev_list_generation << 2)) ||
	    nla_put_u8(msg, NL80211_ATTR_4ADDR, wdev->use_4addr))
		goto nla_put_failure;

	if (rdev->ops->get_channel) {
		int ret;
		struct cfg80211_chan_def chandef = {};

		ret = rdev_get_channel(rdev, wdev, &chandef);
		if (ret == 0) {
			if (nl80211_send_chandef(msg, &chandef))
				goto nla_put_failure;
		}
	}

	if (rdev->ops->get_tx_power) {
		int dbm, ret;

		ret = rdev_get_tx_power(rdev, wdev, &dbm);
		if (ret == 0 &&
		    nla_put_u32(msg, NL80211_ATTR_WIPHY_TX_POWER_LEVEL,
				DBM_TO_MBM(dbm)))
			goto nla_put_failure;
	}

	wdev_lock(wdev);
	switch (wdev->iftype) {
	case NL80211_IFTYPE_AP:
		if (wdev->ssid_len &&
		    nla_put(msg, NL80211_ATTR_SSID, wdev->ssid_len, wdev->ssid))
			goto nla_put_failure_locked;
		break;
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_ADHOC: {
		const u8 *ssid_ie;
		if (!wdev->current_bss)
			break;
		rcu_read_lock();
		ssid_ie = ieee80211_bss_get_ie(&wdev->current_bss->pub,
					       WLAN_EID_SSID);
		if (ssid_ie &&
		    nla_put(msg, NL80211_ATTR_SSID, ssid_ie[1], ssid_ie + 2))
			goto nla_put_failure_rcu_locked;
		rcu_read_unlock();
		break;
		}
	default:
		/* nothing */
		break;
	}
	wdev_unlock(wdev);

	if (rdev->ops->get_txq_stats) {
		struct cfg80211_txq_stats txqstats = {};
		int ret = rdev_get_txq_stats(rdev, wdev, &txqstats);

		if (ret == 0 &&
		    !nl80211_put_txq_stats(msg, &txqstats,
					   NL80211_ATTR_TXQ_STATS))
			goto nla_put_failure;
	}

	genlmsg_end(msg, hdr);
	return 0;

 nla_put_failure_rcu_locked:
	rcu_read_unlock();
 nla_put_failure_locked:
	wdev_unlock(wdev);
 nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}