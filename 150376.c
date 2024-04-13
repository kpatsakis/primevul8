static size_t rtnl_link_get_slave_info_data_size(const struct net_device *dev)
{
	struct net_device *master_dev;
	const struct rtnl_link_ops *ops;

	master_dev = netdev_master_upper_dev_get((struct net_device *) dev);
	if (!master_dev)
		return 0;
	ops = master_dev->rtnl_link_ops;
	if (!ops || !ops->get_slave_size)
		return 0;
	/* IFLA_INFO_SLAVE_DATA + nested data */
	return nla_total_size(sizeof(struct nlattr)) +
	       ops->get_slave_size(master_dev, dev);
}