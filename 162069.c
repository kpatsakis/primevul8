static int nl80211_dump_mpath(struct sk_buff *skb,
			      struct netlink_callback *cb)
{
	struct mpath_info pinfo;
	struct cfg80211_registered_device *dev;
	struct net_device *netdev;
	u8 dst[ETH_ALEN];
	u8 next_hop[ETH_ALEN];
	int path_idx = cb->args[1];
	int err;

	err = nl80211_prepare_netdev_dump(skb, cb, &dev, &netdev);
	if (err)
		return err;

	if (!dev->ops->dump_mpath) {
		err = -EOPNOTSUPP;
		goto out_err;
	}

	if (netdev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT) {
		err = -EOPNOTSUPP;
		goto out_err;
	}

	while (1) {
		err = rdev_dump_mpath(dev, netdev, path_idx, dst, next_hop,
				      &pinfo);
		if (err == -ENOENT)
			break;
		if (err)
			goto out_err;

		if (nl80211_send_mpath(skb, NETLINK_CB(cb->skb).portid,
				       cb->nlh->nlmsg_seq, NLM_F_MULTI,
				       netdev, dst, next_hop,
				       &pinfo) < 0)
			goto out;

		path_idx++;
	}


 out:
	cb->args[1] = path_idx;
	err = skb->len;
 out_err:
	nl80211_finish_netdev_dump(dev);
	return err;
}