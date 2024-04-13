int arc_emac_probe(struct net_device *ndev, int interface)
{
	struct device *dev = ndev->dev.parent;
	struct resource res_regs;
	struct device_node *phy_node;
	struct arc_emac_priv *priv;
	const char *mac_addr;
	unsigned int id, clock_frequency, irq;
	int err;


	/* Get PHY from device tree */
	phy_node = of_parse_phandle(dev->of_node, "phy", 0);
	if (!phy_node) {
		dev_err(dev, "failed to retrieve phy description from device tree\n");
		return -ENODEV;
	}

	/* Get EMAC registers base address from device tree */
	err = of_address_to_resource(dev->of_node, 0, &res_regs);
	if (err) {
		dev_err(dev, "failed to retrieve registers base from device tree\n");
		return -ENODEV;
	}

	/* Get IRQ from device tree */
	irq = irq_of_parse_and_map(dev->of_node, 0);
	if (!irq) {
		dev_err(dev, "failed to retrieve <irq> value from device tree\n");
		return -ENODEV;
	}


	ndev->netdev_ops = &arc_emac_netdev_ops;
	ndev->ethtool_ops = &arc_emac_ethtool_ops;
	ndev->watchdog_timeo = TX_TIMEOUT;
	/* FIXME :: no multicast support yet */
	ndev->flags &= ~IFF_MULTICAST;

	priv = netdev_priv(ndev);
	priv->dev = dev;

	priv->regs = devm_ioremap_resource(dev, &res_regs);
	if (IS_ERR(priv->regs)) {
		return PTR_ERR(priv->regs);
	}
	dev_dbg(dev, "Registers base address is 0x%p\n", priv->regs);

	if (priv->clk) {
		err = clk_prepare_enable(priv->clk);
		if (err) {
			dev_err(dev, "failed to enable clock\n");
			return err;
		}

		clock_frequency = clk_get_rate(priv->clk);
	} else {
		/* Get CPU clock frequency from device tree */
		if (of_property_read_u32(dev->of_node, "clock-frequency",
					 &clock_frequency)) {
			dev_err(dev, "failed to retrieve <clock-frequency> from device tree\n");
			return -EINVAL;
		}
	}

	id = arc_reg_get(priv, R_ID);

	/* Check for EMAC revision 5 or 7, magic number */
	if (!(id == 0x0005fd02 || id == 0x0007fd02)) {
		dev_err(dev, "ARC EMAC not detected, id=0x%x\n", id);
		err = -ENODEV;
		goto out_clken;
	}
	dev_info(dev, "ARC EMAC detected with id: 0x%x\n", id);

	/* Set poll rate so that it polls every 1 ms */
	arc_reg_set(priv, R_POLLRATE, clock_frequency / 1000000);

	ndev->irq = irq;
	dev_info(dev, "IRQ is %d\n", ndev->irq);

	/* Register interrupt handler for device */
	err = devm_request_irq(dev, ndev->irq, arc_emac_intr, 0,
			       ndev->name, ndev);
	if (err) {
		dev_err(dev, "could not allocate IRQ\n");
		goto out_clken;
	}

	/* Get MAC address from device tree */
	mac_addr = of_get_mac_address(dev->of_node);

	if (mac_addr)
		memcpy(ndev->dev_addr, mac_addr, ETH_ALEN);
	else
		eth_hw_addr_random(ndev);

	arc_emac_set_address_internal(ndev);
	dev_info(dev, "MAC address is now %pM\n", ndev->dev_addr);

	/* Do 1 allocation instead of 2 separate ones for Rx and Tx BD rings */
	priv->rxbd = dmam_alloc_coherent(dev, RX_RING_SZ + TX_RING_SZ,
					 &priv->rxbd_dma, GFP_KERNEL);

	if (!priv->rxbd) {
		dev_err(dev, "failed to allocate data buffers\n");
		err = -ENOMEM;
		goto out_clken;
	}

	priv->txbd = priv->rxbd + RX_BD_NUM;

	priv->txbd_dma = priv->rxbd_dma + RX_RING_SZ;
	dev_dbg(dev, "EMAC Device addr: Rx Ring [0x%x], Tx Ring[%x]\n",
		(unsigned int)priv->rxbd_dma, (unsigned int)priv->txbd_dma);

	err = arc_mdio_probe(priv);
	if (err) {
		dev_err(dev, "failed to probe MII bus\n");
		goto out_clken;
	}

	priv->phy_dev = of_phy_connect(ndev, phy_node, arc_emac_adjust_link, 0,
				       interface);
	if (!priv->phy_dev) {
		dev_err(dev, "of_phy_connect() failed\n");
		err = -ENODEV;
		goto out_mdio;
	}

	dev_info(dev, "connected to %s phy with id 0x%x\n",
		 priv->phy_dev->drv->name, priv->phy_dev->phy_id);

	netif_napi_add(ndev, &priv->napi, arc_emac_poll, ARC_EMAC_NAPI_WEIGHT);

	err = register_netdev(ndev);
	if (err) {
		dev_err(dev, "failed to register network device\n");
		goto out_netif_api;
	}

	return 0;

out_netif_api:
	netif_napi_del(&priv->napi);
	phy_disconnect(priv->phy_dev);
	priv->phy_dev = NULL;
out_mdio:
	arc_mdio_remove(priv);
out_clken:
	if (priv->clk)
		clk_disable_unprepare(priv->clk);
	return err;
}