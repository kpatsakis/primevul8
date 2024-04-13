static int usb_internal_control_msg(struct usb_device *usb_dev,
				    unsigned int pipe,
				    struct usb_ctrlrequest *cmd,
				    void *data, int len, int timeout)
{
	struct urb *urb;
	int retv;
	int length;

	urb = usb_alloc_urb(0, GFP_NOIO);
	if (!urb)
		return -ENOMEM;

	usb_fill_control_urb(urb, usb_dev, pipe, (unsigned char *)cmd, data,
			     len, usb_api_blocking_completion, NULL);

	retv = usb_start_wait_urb(urb, timeout, &length);
	if (retv < 0)
		return retv;
	else
		return length;
}