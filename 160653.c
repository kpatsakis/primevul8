ieee802154_get_dev(struct net *net, const struct ieee802154_addr *addr)
{
	struct net_device *dev = NULL;
	struct net_device *tmp;
	__le16 pan_id, short_addr;
	u8 hwaddr[IEEE802154_ADDR_LEN];

	switch (addr->mode) {
	case IEEE802154_ADDR_LONG:
		ieee802154_devaddr_to_raw(hwaddr, addr->extended_addr);
		rcu_read_lock();
		dev = dev_getbyhwaddr_rcu(net, ARPHRD_IEEE802154, hwaddr);
		if (dev)
			dev_hold(dev);
		rcu_read_unlock();
		break;
	case IEEE802154_ADDR_SHORT:
		if (addr->pan_id == cpu_to_le16(IEEE802154_PANID_BROADCAST) ||
		    addr->short_addr == cpu_to_le16(IEEE802154_ADDR_UNDEF) ||
		    addr->short_addr == cpu_to_le16(IEEE802154_ADDR_BROADCAST))
			break;

		rtnl_lock();

		for_each_netdev(net, tmp) {
			if (tmp->type != ARPHRD_IEEE802154)
				continue;

			pan_id = tmp->ieee802154_ptr->pan_id;
			short_addr = tmp->ieee802154_ptr->short_addr;
			if (pan_id == addr->pan_id &&
			    short_addr == addr->short_addr) {
				dev = tmp;
				dev_hold(dev);
				break;
			}
		}

		rtnl_unlock();
		break;
	default:
		pr_warn("Unsupported ieee802154 address type: %d\n",
			addr->mode);
		break;
	}

	return dev;
}