nfp_flower_repr_netdev_open(struct nfp_app *app, struct nfp_repr *repr)
{
	int err;

	err = nfp_flower_cmsg_portmod(repr, true, repr->netdev->mtu, false);
	if (err)
		return err;

	netif_tx_wake_all_queues(repr->netdev);

	return 0;
}