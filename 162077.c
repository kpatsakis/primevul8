static int nl80211_get_wiphy(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *msg;
	struct cfg80211_registered_device *dev = info->user_ptr[0];

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	if (nl80211_send_wiphy(msg, info->snd_portid, info->snd_seq, 0, dev) < 0) {
		nlmsg_free(msg);
		return -ENOBUFS;
	}

	return genlmsg_reply(msg, info);
}