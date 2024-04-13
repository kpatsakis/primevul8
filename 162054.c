static int nl80211_wiphy_netns(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct net *net;
	int err;
	u32 pid;

	if (!info->attrs[NL80211_ATTR_PID])
		return -EINVAL;

	pid = nla_get_u32(info->attrs[NL80211_ATTR_PID]);

	net = get_net_ns_by_pid(pid);
	if (IS_ERR(net))
		return PTR_ERR(net);

	err = 0;

	/* check if anything to do */
	if (!net_eq(wiphy_net(&rdev->wiphy), net))
		err = cfg80211_switch_netns(rdev, net);

	put_net(net);
	return err;
}