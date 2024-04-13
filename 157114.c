static int create_standard_audio_quirk(struct snd_usb_audio *chip,
				       struct usb_interface *iface,
				       struct usb_driver *driver,
				       const struct snd_usb_audio_quirk *quirk)
{
	struct usb_host_interface *alts;
	struct usb_interface_descriptor *altsd;
	int err;

	if (chip->usb_id == USB_ID(0x1686, 0x00dd)) /* Zoom R16/24 */
		chip->tx_length_quirk = 1;

	alts = &iface->altsetting[0];
	altsd = get_iface_desc(alts);
	err = snd_usb_parse_audio_interface(chip, altsd->bInterfaceNumber);
	if (err < 0) {
		usb_audio_err(chip, "cannot setup if %d: error %d\n",
			   altsd->bInterfaceNumber, err);
		return err;
	}
	/* reset the current interface */
	usb_set_interface(chip->dev, altsd->bInterfaceNumber, 0);
	return 0;
}