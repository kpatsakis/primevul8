static int arc_emac_stop(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	napi_disable(&priv->napi);
	netif_stop_queue(ndev);

	/* Disable interrupts */
	arc_reg_clr(priv, R_ENABLE, RXINT_MASK | TXINT_MASK | ERR_MASK);

	/* Disable EMAC */
	arc_reg_clr(priv, R_CTRL, EN_MASK);

	return 0;
}