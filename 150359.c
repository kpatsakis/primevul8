static bool rtnl_have_link_slave_info(const struct net_device *dev)
{
	struct net_device *master_dev;

	master_dev = netdev_master_upper_dev_get((struct net_device *) dev);
	if (master_dev && master_dev->rtnl_link_ops)
		return true;
	return false;
}