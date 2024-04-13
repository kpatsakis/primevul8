static int hso_stop_net_device(struct hso_device *hso_dev)
{
	int i;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return -ENODEV;

	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {
		if (hso_net->mux_bulk_rx_urb_pool[i])
			usb_kill_urb(hso_net->mux_bulk_rx_urb_pool[i]);

	}
	if (hso_net->mux_bulk_tx_urb)
		usb_kill_urb(hso_net->mux_bulk_tx_urb);

	return 0;
}