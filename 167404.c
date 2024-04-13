static struct usb_endpoint_descriptor *hso_get_ep(struct usb_interface *intf,
						  int type, int dir)
{
	int i;
	struct usb_host_interface *iface = intf->cur_altsetting;
	struct usb_endpoint_descriptor *endp;

	for (i = 0; i < iface->desc.bNumEndpoints; i++) {
		endp = &iface->endpoint[i].desc;
		if (((endp->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == dir) &&
		    (usb_endpoint_type(endp) == type))
			return endp;
	}

	return NULL;
}