static int rtnl_link_slave_info_fill(struct sk_buff *skb,
				     const struct net_device *dev)
{
	struct net_device *master_dev;
	const struct rtnl_link_ops *ops;
	struct nlattr *slave_data;
	int err;

	master_dev = netdev_master_upper_dev_get((struct net_device *) dev);
	if (!master_dev)
		return 0;
	ops = master_dev->rtnl_link_ops;
	if (!ops)
		return 0;
	if (nla_put_string(skb, IFLA_INFO_SLAVE_KIND, ops->kind) < 0)
		return -EMSGSIZE;
	if (ops->fill_slave_info) {
		slave_data = nla_nest_start(skb, IFLA_INFO_SLAVE_DATA);
		if (!slave_data)
			return -EMSGSIZE;
		err = ops->fill_slave_info(skb, master_dev, dev);
		if (err < 0)
			goto err_cancel_slave_data;
		nla_nest_end(skb, slave_data);
	}
	return 0;

err_cancel_slave_data:
	nla_nest_cancel(skb, slave_data);
	return err;
}