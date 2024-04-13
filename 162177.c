static int nl80211_prepare_netdev_dump(struct sk_buff *skb,
				       struct netlink_callback *cb,
				       struct cfg80211_registered_device **rdev,
				       struct net_device **dev)
{
	int ifidx = cb->args[0];
	int err;

	if (!ifidx)
		ifidx = nl80211_get_ifidx(cb);
	if (ifidx < 0)
		return ifidx;

	cb->args[0] = ifidx;

	rtnl_lock();

	*dev = __dev_get_by_index(sock_net(skb->sk), ifidx);
	if (!*dev) {
		err = -ENODEV;
		goto out_rtnl;
	}

	*rdev = cfg80211_get_dev_from_ifindex(sock_net(skb->sk), ifidx);
	if (IS_ERR(*rdev)) {
		err = PTR_ERR(*rdev);
		goto out_rtnl;
	}

	return 0;
 out_rtnl:
	rtnl_unlock();
	return err;
}