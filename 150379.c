static int rtnl_phys_switch_id_fill(struct sk_buff *skb, struct net_device *dev)
{
	int err;
	struct switchdev_attr attr = {
		.orig_dev = dev,
		.id = SWITCHDEV_ATTR_ID_PORT_PARENT_ID,
		.flags = SWITCHDEV_F_NO_RECURSE,
	};

	err = switchdev_port_attr_get(dev, &attr);
	if (err) {
		if (err == -EOPNOTSUPP)
			return 0;
		return err;
	}

	if (nla_put(skb, IFLA_PHYS_SWITCH_ID, attr.u.ppid.id_len,
		    attr.u.ppid.id))
		return -EMSGSIZE;

	return 0;
}