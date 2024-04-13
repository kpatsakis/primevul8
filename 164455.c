nfp_flower_repr_netdev_clean(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_repr *repr = netdev_priv(netdev);

	kfree(repr->app_priv);
}