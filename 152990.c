static int arc_emac_poll(struct napi_struct *napi, int budget)
{
	struct net_device *ndev = napi->dev;
	struct arc_emac_priv *priv = netdev_priv(ndev);
	unsigned int work_done;

	arc_emac_tx_clean(ndev);

	work_done = arc_emac_rx(ndev, budget);
	if (work_done < budget) {
		napi_complete(napi);
		arc_reg_or(priv, R_ENABLE, RXINT_MASK | TXINT_MASK);
	}

	return work_done;
}