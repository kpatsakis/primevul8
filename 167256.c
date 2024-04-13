static int _usbctrl_vendorreq_async_write(struct usb_device *udev, u8 request,
					  u16 value, u16 index, void *pdata,
					  u16 len)
{
	int rc;
	unsigned int pipe;
	u8 reqtype;
	struct usb_ctrlrequest *dr;
	struct urb *urb;
	const u16 databuf_maxlen = REALTEK_USB_VENQT_MAX_BUF_SIZE;
	u8 *databuf;

	if (WARN_ON_ONCE(len > databuf_maxlen))
		len = databuf_maxlen;

	pipe = usb_sndctrlpipe(udev, 0); /* write_out */
	reqtype =  REALTEK_USB_VENQT_WRITE;

	dr = kzalloc(sizeof(*dr), GFP_ATOMIC);
	if (!dr)
		return -ENOMEM;

	databuf = kzalloc(databuf_maxlen, GFP_ATOMIC);
	if (!databuf) {
		kfree(dr);
		return -ENOMEM;
	}

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb) {
		kfree(databuf);
		kfree(dr);
		return -ENOMEM;
	}

	dr->bRequestType = reqtype;
	dr->bRequest = request;
	dr->wValue = cpu_to_le16(value);
	dr->wIndex = cpu_to_le16(index);
	dr->wLength = cpu_to_le16(len);
	/* data are already in little-endian order */
	memcpy(databuf, pdata, len);
	usb_fill_control_urb(urb, udev, pipe,
			     (unsigned char *)dr, databuf, len,
			     usbctrl_async_callback, NULL);
	rc = usb_submit_urb(urb, GFP_ATOMIC);
	if (rc < 0) {
		kfree(databuf);
		kfree(dr);
	}
	usb_free_urb(urb);
	return rc;
}