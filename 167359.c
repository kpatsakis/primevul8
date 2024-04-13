static int mux_device_request(struct hso_serial *serial, u8 type, u16 port,
			      struct urb *ctrl_urb,
			      struct usb_ctrlrequest *ctrl_req,
			      u8 *ctrl_urb_data, u32 size)
{
	int result;
	int pipe;

	/* Sanity check */
	if (!serial || !ctrl_urb || !ctrl_req) {
		pr_err("%s: Wrong arguments\n", __func__);
		return -EINVAL;
	}

	/* initialize */
	ctrl_req->wValue = 0;
	ctrl_req->wIndex = cpu_to_le16(hso_port_to_mux(port));
	ctrl_req->wLength = cpu_to_le16(size);

	if (type == USB_CDC_GET_ENCAPSULATED_RESPONSE) {
		/* Reading command */
		ctrl_req->bRequestType = USB_DIR_IN |
					 USB_TYPE_OPTION_VENDOR |
					 USB_RECIP_INTERFACE;
		ctrl_req->bRequest = USB_CDC_GET_ENCAPSULATED_RESPONSE;
		pipe = usb_rcvctrlpipe(serial->parent->usb, 0);
	} else {
		/* Writing command */
		ctrl_req->bRequestType = USB_DIR_OUT |
					 USB_TYPE_OPTION_VENDOR |
					 USB_RECIP_INTERFACE;
		ctrl_req->bRequest = USB_CDC_SEND_ENCAPSULATED_COMMAND;
		pipe = usb_sndctrlpipe(serial->parent->usb, 0);
	}
	/* syslog */
	hso_dbg(0x2, "%s command (%02x) len: %d, port: %d\n",
		type == USB_CDC_GET_ENCAPSULATED_RESPONSE ? "Read" : "Write",
		ctrl_req->bRequestType, ctrl_req->wLength, port);

	/* Load ctrl urb */
	ctrl_urb->transfer_flags = 0;
	usb_fill_control_urb(ctrl_urb,
			     serial->parent->usb,
			     pipe,
			     (u8 *) ctrl_req,
			     ctrl_urb_data, size, ctrl_callback, serial);
	/* Send it on merry way */
	result = usb_submit_urb(ctrl_urb, GFP_ATOMIC);
	if (result) {
		dev_err(&ctrl_urb->dev->dev,
			"%s failed submit ctrl_urb %d type %d\n", __func__,
			result, type);
		return result;
	}

	/* done */
	return size;
}