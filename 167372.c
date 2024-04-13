static int hso_start_net_device(struct hso_device *hso_dev)
{
	int i, result = 0;
	struct hso_net *hso_net = dev2net(hso_dev);

	if (!hso_net)
		return -ENODEV;

	/* send URBs for all read buffers */
	for (i = 0; i < MUX_BULK_RX_BUF_COUNT; i++) {

		/* Prep a receive URB */
		usb_fill_bulk_urb(hso_net->mux_bulk_rx_urb_pool[i],
				  hso_dev->usb,
				  usb_rcvbulkpipe(hso_dev->usb,
						  hso_net->in_endp->
						  bEndpointAddress & 0x7F),
				  hso_net->mux_bulk_rx_buf_pool[i],
				  MUX_BULK_RX_BUF_SIZE, read_bulk_callback,
				  hso_net);

		/* Put it out there so the device can send us stuff */
		result = usb_submit_urb(hso_net->mux_bulk_rx_urb_pool[i],
					GFP_NOIO);
		if (result)
			dev_warn(&hso_dev->usb->dev,
				"%s failed mux_bulk_rx_urb[%d] %d\n", __func__,
				i, result);
	}

	return result;
}