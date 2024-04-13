static struct hso_device *hso_create_device(struct usb_interface *intf,
					    int port_spec)
{
	struct hso_device *hso_dev;

	hso_dev = kzalloc(sizeof(*hso_dev), GFP_ATOMIC);
	if (!hso_dev)
		return NULL;

	hso_dev->port_spec = port_spec;
	hso_dev->usb = interface_to_usbdev(intf);
	hso_dev->interface = intf;
	kref_init(&hso_dev->ref);
	mutex_init(&hso_dev->mutex);

	INIT_WORK(&hso_dev->async_get_intf, async_get_intf);
	INIT_WORK(&hso_dev->async_put_intf, async_put_intf);

	return hso_dev;
}