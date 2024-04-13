static int hso_net_open(struct net_device *net)
{
	struct hso_net *odev = netdev_priv(net);
	unsigned long flags = 0;

	if (!odev) {
		dev_err(&net->dev, "No net device !\n");
		return -ENODEV;
	}

	odev->skb_tx_buf = NULL;

	/* setup environment */
	spin_lock_irqsave(&odev->net_lock, flags);
	odev->rx_parse_state = WAIT_IP;
	odev->rx_buf_size = 0;
	odev->rx_buf_missing = sizeof(struct iphdr);
	spin_unlock_irqrestore(&odev->net_lock, flags);

	/* We are up and running. */
	set_bit(HSO_NET_RUNNING, &odev->flags);
	hso_start_net_device(odev->parent);

	/* Tell the kernel we are ready to start receiving from it */
	netif_start_queue(net);

	return 0;
}