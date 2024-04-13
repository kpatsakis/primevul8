static void hso_free_net_device(struct hso_device *hso_dev)
{
	int i;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return;

	remove_net_device(hso_net->parent);

	if (hso_net->net)
		unregister_netdev(hso_net->net);

	/* start freeing */
	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {
		usb_free_urb(hso_net->mux_bulk_rx_urb_pool[i]);
		kfree(hso_net->mux_bulk_rx_buf_pool[i]);
		hso_net->mux_bulk_rx_buf_pool[i] = NULL;
	}
	usb_free_urb(hso_net->mux_bulk_tx_urb);
	kfree(hso_net->mux_bulk_tx_buf);
	hso_net->mux_bulk_tx_buf = NULL;

	if (hso_net->net)
		free_netdev(hso_net->net);

	kfree(hso_dev);
}