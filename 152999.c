static irqreturn_t arc_emac_intr(int irq, void *dev_instance)
{
	struct net_device *ndev = dev_instance;
	struct arc_emac_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	unsigned int status;

	status = arc_reg_get(priv, R_STATUS);
	status &= ~MDIO_MASK;

	/* Reset all flags except "MDIO complete" */
	arc_reg_set(priv, R_STATUS, status);

	if (status & (RXINT_MASK | TXINT_MASK)) {
		if (likely(napi_schedule_prep(&priv->napi))) {
			arc_reg_clr(priv, R_ENABLE, RXINT_MASK | TXINT_MASK);
			__napi_schedule(&priv->napi);
		}
	}

	if (status & ERR_MASK) {
		/* MSER/RXCR/RXFR/RXFL interrupt fires on corresponding
		 * 8-bit error counter overrun.
		 */

		if (status & MSER_MASK) {
			stats->rx_missed_errors += 0x100;
			stats->rx_errors += 0x100;
		}

		if (status & RXCR_MASK) {
			stats->rx_crc_errors += 0x100;
			stats->rx_errors += 0x100;
		}

		if (status & RXFR_MASK) {
			stats->rx_frame_errors += 0x100;
			stats->rx_errors += 0x100;
		}

		if (status & RXFL_MASK) {
			stats->rx_over_errors += 0x100;
			stats->rx_errors += 0x100;
		}
	}

	return IRQ_HANDLED;
}