static int arc_emac_get_settings(struct net_device *ndev,
				 struct ethtool_cmd *cmd)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	return phy_ethtool_gset(priv->phy_dev, cmd);
}