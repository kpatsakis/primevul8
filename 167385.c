static int hso_rfkill_set_block(void *data, bool blocked)
{
	struct hso_device *hso_dev = data;
	int enabled = !blocked;
	int rv;

	mutex_lock(&hso_dev->mutex);
	if (hso_dev->usb_gone)
		rv = 0;
	else
		rv = usb_control_msg(hso_dev->usb, usb_rcvctrlpipe(hso_dev->usb, 0),
				       enabled ? 0x82 : 0x81, 0x40, 0, 0, NULL, 0,
				       USB_CTRL_SET_TIMEOUT);
	mutex_unlock(&hso_dev->mutex);
	return rv;
}