static int nl80211_get_key(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int err;
	struct net_device *dev = info->user_ptr[1];
	u8 key_idx = 0;
	const u8 *mac_addr = NULL;
	bool pairwise;
	struct get_key_cookie cookie = {
		.error = 0,
	};
	void *hdr;
	struct sk_buff *msg;

	if (info->attrs[NL80211_ATTR_KEY_IDX])
		key_idx = nla_get_u8(info->attrs[NL80211_ATTR_KEY_IDX]);

	if (key_idx > 5)
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_MAC])
		mac_addr = nla_data(info->attrs[NL80211_ATTR_MAC]);

	pairwise = !!mac_addr;
	if (info->attrs[NL80211_ATTR_KEY_TYPE]) {
		u32 kt = nla_get_u32(info->attrs[NL80211_ATTR_KEY_TYPE]);
		if (kt >= NUM_NL80211_KEYTYPES)
			return -EINVAL;
		if (kt != NL80211_KEYTYPE_GROUP &&
		    kt != NL80211_KEYTYPE_PAIRWISE)
			return -EINVAL;
		pairwise = kt == NL80211_KEYTYPE_PAIRWISE;
	}

	if (!rdev->ops->get_key)
		return -EOPNOTSUPP;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
			     NL80211_CMD_NEW_KEY);
	if (IS_ERR(hdr))
		return PTR_ERR(hdr);

	cookie.msg = msg;
	cookie.idx = key_idx;

	if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, dev->ifindex) ||
	    nla_put_u8(msg, NL80211_ATTR_KEY_IDX, key_idx))
		goto nla_put_failure;
	if (mac_addr &&
	    nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, mac_addr))
		goto nla_put_failure;

	if (pairwise && mac_addr &&
	    !(rdev->wiphy.flags & WIPHY_FLAG_IBSS_RSN))
		return -ENOENT;

	err = rdev_get_key(rdev, dev, key_idx, pairwise, mac_addr, &cookie,
			   get_key_callback);

	if (err)
		goto free_msg;

	if (cookie.error)
		goto nla_put_failure;

	genlmsg_end(msg, hdr);
	return genlmsg_reply(msg, info);

 nla_put_failure:
	err = -ENOBUFS;
 free_msg:
	nlmsg_free(msg);
	return err;
}