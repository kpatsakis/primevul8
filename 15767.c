static int em28xx_usb_resume(struct usb_interface *intf)
{
	struct em28xx *dev;

	dev = usb_get_intfdata(intf);
	if (!dev)
		return 0;
	em28xx_resume_extension(dev);
	return 0;
}