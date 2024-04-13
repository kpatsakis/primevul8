static struct net_device_stats *arc_emac_stats(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	unsigned long miss, rxerr;
	u8 rxcrc, rxfram, rxoflow;

	rxerr = arc_reg_get(priv, R_RXERR);
	miss = arc_reg_get(priv, R_MISS);

	rxcrc = rxerr;
	rxfram = rxerr >> 8;
	rxoflow = rxerr >> 16;

	stats->rx_errors += miss;
	stats->rx_errors += rxcrc + rxfram + rxoflow;

	stats->rx_over_errors += rxoflow;
	stats->rx_frame_errors += rxfram;
	stats->rx_crc_errors += rxcrc;
	stats->rx_missed_errors += miss;

	return stats;
}