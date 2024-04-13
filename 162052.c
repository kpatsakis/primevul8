static int nl80211_probe_client(struct sk_buff *skb,
				struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net_device *dev = info->user_ptr[1];
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct sk_buff *msg;
	void *hdr;
	const u8 *addr;
	u64 cookie;
	int err;

	if (wdev->iftype != NL80211_IFTYPE_AP &&
	    wdev->iftype != NL80211_IFTYPE_P2P_GO)
		return -EOPNOTSUPP;

	if (!info->attrs[NL80211_ATTR_MAC])
		return -EINVAL;

	if (!rdev->ops->probe_client)
		return -EOPNOTSUPP;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
			     NL80211_CMD_PROBE_CLIENT);

	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto free_msg;
	}

	addr = nla_data(info->attrs[NL80211_ATTR_MAC]);

	err = rdev_probe_client(rdev, dev, addr, &cookie);
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