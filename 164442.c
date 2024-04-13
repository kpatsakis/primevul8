nfp_flower_repr_netdev_stop(struct nfp_app *app, struct nfp_repr *repr)
{
	netif_tx_disable(repr->netdev);

	return nfp_flower_cmsg_portmod(repr, false, repr->netdev->mtu, false);
}