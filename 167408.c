static int tiocmget_submit_urb(struct hso_serial *serial,
			       struct hso_tiocmget *tiocmget,
			       struct usb_device *usb)
{
	int result;

	if (serial->parent->usb_gone)
		return -ENODEV;
	usb_fill_int_urb(tiocmget->urb, usb,
			 usb_rcvintpipe(usb,
					tiocmget->endp->
					bEndpointAddress & 0x7F),
			 &tiocmget->serial_state_notification,
			 sizeof(struct hso_serial_state_notification),
			 tiocmget_intr_callback, serial,
			 tiocmget->endp->bInterval);
	result = usb_submit_urb(tiocmget->urb, GFP_ATOMIC);
	if (result) {
		dev_warn(&usb->dev, "%s usb_submit_urb failed %d\n", __func__,
			 result);
	}
	return result;

}