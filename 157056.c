static void gs_usb_xmit_callback(struct urb *urb)
{
	struct gs_tx_context *txc = urb->context;
	struct gs_can *dev = txc->dev;
	struct net_device *netdev = dev->netdev;

	if (urb->status)
		netdev_info(netdev, "usb xmit fail %d\n", txc->echo_id);

	usb_free_coherent(urb->dev,
			  urb->transfer_buffer_length,
			  urb->transfer_buffer,
			  urb->transfer_dma);
}