static inline int arc_emac_tx_avail(struct arc_emac_priv *priv)
{
	return (priv->txbd_dirty + TX_BD_NUM - priv->txbd_curr - 1) % TX_BD_NUM;
}