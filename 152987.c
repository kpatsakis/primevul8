static int arc_emac_set_settings(struct net_device *ndev,
				 struct ethtool_cmd *cmd)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	return phy_ethtool_sset(priv->phy_dev, cmd);
}