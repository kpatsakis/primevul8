static int create_autodetect_quirks(struct snd_usb_audio *chip,
				    struct usb_interface *iface,
				    struct usb_driver *driver,
				    const struct snd_usb_audio_quirk *quirk)
{
	int probed_ifnum = get_iface_desc(iface->altsetting)->bInterfaceNumber;
	int ifcount, ifnum, err;

	err = create_autodetect_quirk(chip, iface, driver);
	if (err < 0)
		return err;

	/*
	 * ALSA PCM playback/capture devices cannot be registered in two steps,
	 * so we have to claim the other corresponding interface here.
	 */
	ifcount = chip->dev->actconfig->desc.bNumInterfaces;
	for (ifnum = 0; ifnum < ifcount; ifnum++) {
		if (ifnum == probed_ifnum || quirk->ifnum >= 0)
			continue;
		iface = usb_ifnum_to_if(chip->dev, ifnum);
		if (!iface ||
		    usb_interface_claimed(iface) ||
		    get_iface_desc(iface->altsetting)->bInterfaceClass !=
							USB_CLASS_VENDOR_SPEC)
			continue;

		err = create_autodetect_quirk(chip, iface, driver);
		if (err >= 0)
			usb_driver_claim_interface(driver, iface, (void *)-1L);
	}

	return 0;
}