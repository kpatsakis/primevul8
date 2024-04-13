void snd_usb_set_interface_quirk(struct usb_device *dev)
{
	struct snd_usb_audio *chip = dev_get_drvdata(&dev->dev);

	if (!chip)
		return;
	/*
	 * "Playback Design" products need a 50ms delay after setting the
	 * USB interface.
	 */
	switch (USB_ID_VENDOR(chip->usb_id)) {
	case 0x23ba: /* Playback Design */
	case 0x0644: /* TEAC Corp. */
		mdelay(50);
		break;
	}
}