static void gs_usb_disconnect(struct usb_interface *intf)
{
	unsigned i;
	struct gs_usb *dev = usb_get_intfdata(intf);
	usb_set_intfdata(intf, NULL);

	if (!dev) {
		dev_err(&intf->dev, "Disconnect (nodata)\n");
		return;
	}

	for (i = 0; i < GS_MAX_INTF; i++)
		if (dev->canch[i])
			gs_destroy_candev(dev->canch[i]);

	usb_kill_anchored_urbs(&dev->rx_submitted);
	kfree(dev);
}