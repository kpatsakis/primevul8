static void hso_net_init(struct net_device *net)
{
	struct hso_net *hso_net = netdev_priv(net);

	hso_dbg(0x1, "sizeof hso_net is %zu\n", sizeof(*hso_net));

	/* fill in the other fields */
	net->netdev_ops = &hso_netdev_ops;
	net->watchdog_timeo = HSO_NET_TX_TIMEOUT;
	net->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	net->type = ARPHRD_NONE;
	net->mtu = DEFAULT_MTU - 14;
	net->tx_queue_len = 10;
	net->ethtool_ops = &ops;

	/* and initialize the semaphore */
	spin_lock_init(&hso_net->net_lock);
}