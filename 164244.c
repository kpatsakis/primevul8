static void atif_drop_device(struct net_device *dev)
{
	struct atalk_iface **iface = &atalk_interfaces;
	struct atalk_iface *tmp;

	write_lock_bh(&atalk_interfaces_lock);
	while ((tmp = *iface) != NULL) {
		if (tmp->dev == dev) {
			*iface = tmp->next;
			dev_put(dev);
			kfree(tmp);
			dev->atalk_ptr = NULL;
		} else
			iface = &tmp->next;
	}
	write_unlock_bh(&atalk_interfaces_lock);
}