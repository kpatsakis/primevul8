static int arc_emac_set_address(struct net_device *ndev, void *p)
{
	struct sockaddr *addr = p;

	if (netif_running(ndev))
		return -EBUSY;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(ndev->dev_addr, addr->sa_data, ndev->addr_len);

	arc_emac_set_address_internal(ndev);

	return 0;
}