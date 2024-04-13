static void arc_emac_adjust_link(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);
	struct phy_device *phy_dev = priv->phy_dev;
	unsigned int reg, state_changed = 0;

	if (priv->link != phy_dev->link) {
		priv->link = phy_dev->link;
		state_changed = 1;
	}

	if (priv->speed != phy_dev->speed) {
		priv->speed = phy_dev->speed;
		state_changed = 1;
		if (priv->set_mac_speed)
			priv->set_mac_speed(priv, priv->speed);
	}

	if (priv->duplex != phy_dev->duplex) {
		reg = arc_reg_get(priv, R_CTRL);

		if (DUPLEX_FULL == phy_dev->duplex)
			reg |= ENFL_MASK;
		else
			reg &= ~ENFL_MASK;

		arc_reg_set(priv, R_CTRL, reg);
		priv->duplex = phy_dev->duplex;
		state_changed = 1;
	}

	if (state_changed)
		phy_print_status(phy_dev);
}