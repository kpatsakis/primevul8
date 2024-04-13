static void async_put_intf(struct work_struct *data)
{
	struct hso_device *hso_dev =
	    container_of(data, struct hso_device, async_put_intf);
	usb_autopm_put_interface(hso_dev->interface);
}