static int hso_put_activity(struct hso_device *hso_dev)
{
	if (hso_dev->usb->state != USB_STATE_SUSPENDED) {
		if (hso_dev->is_active) {
			hso_dev->is_active = 0;
			schedule_work(&hso_dev->async_put_intf);
			return -EAGAIN;
		}
	}
	hso_dev->is_active = 0;
	return 0;
}