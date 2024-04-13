static int create_auto_midi_quirk(struct snd_usb_audio *chip,
				  struct usb_interface *iface,
				  struct usb_driver *driver)
{
	struct usb_host_interface *alts;
	struct usb_interface_descriptor *altsd;
	struct usb_endpoint_descriptor *epd;
	int err;

	alts = &iface->altsetting[0];
	altsd = get_iface_desc(alts);

	/* must have at least one bulk/interrupt endpoint for streaming */
	if (altsd->bNumEndpoints < 1)
		return -ENODEV;
	epd = get_endpoint(alts, 0);
	if (!usb_endpoint_xfer_bulk(epd) &&
	    !usb_endpoint_xfer_int(epd))
		return -ENODEV;

	switch (USB_ID_VENDOR(chip->usb_id)) {
	case 0x0499: /* Yamaha */
		err = create_yamaha_midi_quirk(chip, iface, driver, alts);
		if (err != -ENODEV)
			return err;
		break;
	case 0x0582: /* Roland */
		err = create_roland_midi_quirk(chip, iface, driver, alts);
		if (err != -ENODEV)
			return err;
		break;
	}

	return create_std_midi_quirk(chip, iface, driver, alts);
}