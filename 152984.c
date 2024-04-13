static void arc_emac_get_drvinfo(struct net_device *ndev,
				 struct ethtool_drvinfo *info)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	strlcpy(info->driver, priv->drv_name, sizeof(info->driver));
	strlcpy(info->version, priv->drv_version, sizeof(info->version));
}