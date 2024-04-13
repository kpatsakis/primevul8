static void async_get_intf(struct work_struct *data)
{
	struct hso_device *hso_dev =
	    container_of(data, struct hso_device, async_get_intf);
	usb_autopm_get_interface(hso_dev->interface);
}