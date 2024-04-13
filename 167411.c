static void hso_create_rfkill(struct hso_device *hso_dev,
			     struct usb_interface *interface)
{
	struct hso_net *hso_net = dev2net(hso_dev);
	struct device *dev = &hso_net->net->dev;
	static u32 rfkill_counter;

	snprintf(hso_net->name, sizeof(hso_net->name), "hso-%d",
		 rfkill_counter++);

	hso_net->rfkill = rfkill_alloc(hso_net->name,
				       &interface_to_usbdev(interface)->dev,
				       RFKILL_TYPE_WWAN,
				       &hso_rfkill_ops, hso_dev);
	if (!hso_net->rfkill) {
		dev_err(dev, "%s - Out of memory\n", __func__);
		return;
	}
	if (rfkill_register(hso_net->rfkill) < 0) {
		rfkill_destroy(hso_net->rfkill);
		hso_net->rfkill = NULL;
		dev_err(dev, "%s - Failed to register rfkill\n", __func__);
		return;
	}
}