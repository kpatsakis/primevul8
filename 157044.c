static int gs_usb_set_identify(struct net_device *netdev, bool do_identify)
{
	struct gs_can *dev = netdev_priv(netdev);
	struct gs_identify_mode *imode;
	int rc;

	imode = kmalloc(sizeof(*imode), GFP_KERNEL);

	if (!imode)
		return -ENOMEM;

	if (do_identify)
		imode->mode = GS_CAN_IDENTIFY_ON;
	else
		imode->mode = GS_CAN_IDENTIFY_OFF;

	rc = usb_control_msg(interface_to_usbdev(dev->iface),
			     usb_sndctrlpipe(interface_to_usbdev(dev->iface),
					     0),
			     GS_USB_BREQ_IDENTIFY,
			     USB_DIR_OUT | USB_TYPE_VENDOR |
			     USB_RECIP_INTERFACE,
			     dev->channel,
			     0,
			     imode,
			     sizeof(*imode),
			     100);

	kfree(imode);

	return (rc > 0) ? 0 : rc;
}