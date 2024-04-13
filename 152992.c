static void arc_emac_set_rx_mode(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);

	if (ndev->flags & IFF_PROMISC) {
		arc_reg_or(priv, R_CTRL, PROM_MASK);
	} else {
		arc_reg_clr(priv, R_CTRL, PROM_MASK);

		if (ndev->flags & IFF_ALLMULTI) {
			arc_reg_set(priv, R_LAFL, ~0);
			arc_reg_set(priv, R_LAFH, ~0);
		} else {
			struct netdev_hw_addr *ha;
			unsigned int filter[2] = { 0, 0 };
			int bit;

			netdev_for_each_mc_addr(ha, ndev) {
				bit = ether_crc_le(ETH_ALEN, ha->addr) >> 26;
				filter[bit >> 5] |= 1 << (bit & 31);
			}

			arc_reg_set(priv, R_LAFL, filter[0]);
			arc_reg_set(priv, R_LAFH, filter[1]);
		}
	}
}