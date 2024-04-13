static void hso_disconnect(struct usb_interface *interface)
{
	hso_free_interface(interface);

	/* remove reference of our private data */
	usb_set_intfdata(interface, NULL);
}