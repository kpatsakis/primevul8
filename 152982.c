int arc_emac_remove(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	phy_disconnect(priv->phy_dev);
	priv->phy_dev = NULL;
	arc_mdio_remove(priv);
	unregister_netdev(ndev);
	netif_napi_del(&priv->napi);

	if (!IS_ERR(priv->clk)) {
		clk_disable_unprepare(priv->clk);
	}


	return 0;
}