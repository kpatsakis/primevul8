static int rtnl_bridge_dellink(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	struct ifinfomsg *ifm;
	struct net_device *dev;
	struct nlattr *br_spec, *attr = NULL;
	int rem, err = -EOPNOTSUPP;
	u16 flags = 0;
	bool have_flags = false;

	if (nlmsg_len(nlh) < sizeof(*ifm))
		return -EINVAL;

	ifm = nlmsg_data(nlh);
	if (ifm->ifi_family != AF_BRIDGE)
		return -EPFNOSUPPORT;

	dev = __dev_get_by_index(net, ifm->ifi_index);
	if (!dev) {
		pr_info("PF_BRIDGE: RTM_SETLINK with unknown ifindex\n");
		return -ENODEV;
	}

	br_spec = nlmsg_find_attr(nlh, sizeof(struct ifinfomsg), IFLA_AF_SPEC);
	if (br_spec) {
		nla_for_each_nested(attr, br_spec, rem) {
			if (nla_type(attr) == IFLA_BRIDGE_FLAGS) {
				if (nla_len(attr) < sizeof(flags))
					return -EINVAL;

				have_flags = true;
				flags = nla_get_u16(attr);
				break;
			}
		}
	}

	if (!flags || (flags & BRIDGE_FLAGS_MASTER)) {
		struct net_device *br_dev = netdev_master_upper_dev_get(dev);

		if (!br_dev || !br_dev->netdev_ops->ndo_bridge_dellink) {
			err = -EOPNOTSUPP;
			goto out;
		}

		err = br_dev->netdev_ops->ndo_bridge_dellink(dev, nlh, flags);
		if (err)
			goto out;

		flags &= ~BRIDGE_FLAGS_MASTER;
	}

	if ((flags & BRIDGE_FLAGS_SELF)) {
		if (!dev->netdev_ops->ndo_bridge_dellink)
			err = -EOPNOTSUPP;
		else
			err = dev->netdev_ops->ndo_bridge_dellink(dev, nlh,
								  flags);

		if (!err) {
			flags &= ~BRIDGE_FLAGS_SELF;

			/* Generate event to notify upper layer of bridge
			 * change
			 */
			err = rtnl_bridge_notify(dev);
		}
	}

	if (have_flags)
		memcpy(nla_data(attr), &flags, sizeof(flags));
out:
	return err;
}