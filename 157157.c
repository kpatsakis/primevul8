static int snd_usb_cm106_boot_quirk(struct usb_device *dev)
{
	/*
	 * Enable line-out driver mode, set headphone source to front
	 * channels, enable stereo mic.
	 */
	return snd_usb_cm106_write_int_reg(dev, 2, 0x8004);
}