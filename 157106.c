static int snd_usb_novation_boot_quirk(struct usb_device *dev)
{
	/* preemptively set up the device because otherwise the
	 * raw MIDI endpoints are not active */
	usb_set_interface(dev, 0, 1);
	return 0;
}