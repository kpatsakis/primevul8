static void write_bulk_callback(struct urb *urb)
{
	struct hso_net *odev = urb->context;
	int status = urb->status;

	/* Sanity check */
	if (!odev || !test_bit(HSO_NET_RUNNING, &odev->flags)) {
		dev_err(&urb->dev->dev, "%s: device not running\n", __func__);
		return;
	}

	/* Do we still have a valid kernel network device? */
	if (!netif_device_present(odev->net)) {
		dev_err(&urb->dev->dev, "%s: net device not present\n",
			__func__);
		return;
	}

	/* log status, but don't act on it, we don't need to resubmit anything
	 * anyhow */
	if (status)
		handle_usb_error(status, __func__, odev->parent);

	hso_put_activity(odev->parent);

	/* Tell the network interface we are ready for another frame */
	netif_wake_queue(odev->net);
}