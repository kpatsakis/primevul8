static int nl80211_tx_mgmt(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev = info->user_ptr[1];
	struct cfg80211_chan_def chandef;
	int err;
	void *hdr = NULL;
	u64 cookie;
	struct sk_buff *msg = NULL;
	unsigned int wait = 0;
	bool offchan, no_cck, dont_wait_for_ack;

	dont_wait_for_ack = info->attrs[NL80211_ATTR_DONT_WAIT_FOR_ACK];

	if (!info->attrs[NL80211_ATTR_FRAME])
		return -EINVAL;

	if (!rdev->ops->mgmt_tx)
		return -EOPNOTSUPP;

	switch (wdev->iftype) {
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_ADHOC:
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_MESH_POINT:
	case NL80211_IFTYPE_P2P_GO:
	case NL80211_IFTYPE_P2P_DEVICE:
		break;
	default:
		return -EOPNOTSUPP;
	}

	if (info->attrs[NL80211_ATTR_DURATION]) {
		if (!(rdev->wiphy.flags & WIPHY_FLAG_OFFCHAN_TX))
			return -EINVAL;
		wait = nla_get_u32(info->attrs[NL80211_ATTR_DURATION]);

		/*
		 * We should wait on the channel for at least a minimum amount
		 * of time (10ms) but no longer than the driver supports.
		 */
		if (wait < NL80211_MIN_REMAIN_ON_CHANNEL_TIME ||
		    wait > rdev->wiphy.max_remain_on_channel_duration)
			return -EINVAL;

	}

	offchan = info->attrs[NL80211_ATTR_OFFCHANNEL_TX_OK];

	if (offchan && !(rdev->wiphy.flags & WIPHY_FLAG_OFFCHAN_TX))
		return -EINVAL;

	no_cck = nla_get_flag(info->attrs[NL80211_ATTR_TX_NO_CCK_RATE]);

	err = nl80211_parse_chandef(rdev, info, &chandef);
	if (err)
		return err;

	if (!dont_wait_for_ack) {
		msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
		if (!msg)
			return -ENOMEM;

		hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
				     NL80211_CMD_FRAME);

		if (IS_ERR(hdr)) {
			err = PTR_ERR(hdr);
			goto free_msg;
		}
	}

	err = cfg80211_mlme_mgmt_tx(rdev, wdev, chandef.chan, offchan, wait,
				    nla_data(info->attrs[NL80211_ATTR_FRAME]),
				    nla_len(info->attrs[NL80211_ATTR_FRAME]),
				    no_cck, dont_wait_for_ack, &cookie);
	if (err)
		goto free_msg;

	if (msg) {
		if (nla_put_u64(msg, NL80211_ATTR_COOKIE, cookie))
			goto nla_put_failure;

		genlmsg_end(msg, hdr);
		return genlmsg_reply(msg, info);
	}

	return 0;

 nla_put_failure:
	err = -ENOBUFS;
 free_msg:
	nlmsg_free(msg);
	return err;
}