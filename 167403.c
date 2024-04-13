static int hso_get_mux_ports(struct usb_interface *intf, unsigned char *ports)
{
	int i;
	struct usb_host_interface *iface = intf->cur_altsetting;

	if (iface->extralen == 3) {
		*ports = iface->extra[2];
		return 0;
	}

	for (i = 0; i < iface->desc.bNumEndpoints; i++) {
		if (iface->endpoint[i].extralen == 3) {
			*ports = iface->endpoint[i].extra[2];
			return 0;
		}
	}

	return -1;
}