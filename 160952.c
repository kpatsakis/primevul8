static int write_packet(struct usb_device *udev,
			u8 request, u8 * registers, u16 start, size_t size)
{
	unsigned char *buf;
	int ret;

	if (!registers || size <= 0)
		return -EINVAL;

	buf = kmemdup(registers, size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = usb_control_msg(udev,
			       usb_sndctrlpipe(udev, 0),
			       request,
			       USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			       start,	/* value */
			       0,	/* index */
			       buf,	/* buffer */
			       size,
			       HZ);

	kfree(buf);
	return ret;
}