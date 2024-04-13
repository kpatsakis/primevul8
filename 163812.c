static void ppp_setup(struct net_device *dev)
{
	dev->netdev_ops = &ppp_netdev_ops;
	dev->hard_header_len = PPP_HDRLEN;
	dev->mtu = PPP_MTU;
	dev->addr_len = 0;
	dev->tx_queue_len = 3;
	dev->type = ARPHRD_PPP;
	dev->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	dev->features |= NETIF_F_NETNS_LOCAL;
}