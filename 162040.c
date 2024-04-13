static int nl80211_remain_on_channel(struct sk_buff *skb,
				     struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev = info->user_ptr[1];
	struct cfg80211_chan_def chandef;
	struct sk_buff *msg;
	void *hdr;
	u64 cookie;
	u32 duration;
	int err;

	if (!info->attrs[NL80211_ATTR_WIPHY_FREQ] ||
	    !info->attrs[NL80211_ATTR_DURATION])
		return -EINVAL;

	duration = nla_get_u32(info->attrs[NL80211_ATTR_DURATION]);

	if (!rdev->ops->remain_on_channel ||
	    !(rdev->wiphy.flags & WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL))
		return -EOPNOTSUPP;

	/*
	 * We should be on that channel for at least a minimum amount of
	 * time (10ms) but no longer than the driver supports.
	 */
	if (duration < NL80211_MIN_REMAIN_ON_CHANNEL_TIME ||
	    duration > rdev->wiphy.max_remain_on_channel_duration)
		return -EINVAL;

	err = nl80211_parse_chandef(rdev, info, &chandef);
	if (err)
		return err;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
			     NL80211_CMD_REMAIN_ON_CHANNEL);

	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto free_msg;
	}

	err = rdev_remain_on_channel(rdev, wdev, chandef.chan,
				     duration, &cookie);

	if (err)
		goto free_msg;

	if (nla_put_u64(msg, NL80211_ATTR_COOKIE, cookie))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	return genlmsg_reply(msg, info);

 nla_put_failure:
	err = -ENOBUFS;
 free_msg:
	nlmsg_free(msg);
	return err;
}