static int create_composite_quirk(struct snd_usb_audio *chip,
				  struct usb_interface *iface,
				  struct usb_driver *driver,
				  const struct snd_usb_audio_quirk *quirk_comp)
{
	int probed_ifnum = get_iface_desc(iface->altsetting)->bInterfaceNumber;
	const struct snd_usb_audio_quirk *quirk;
	int err;

	for (quirk = quirk_comp->data; quirk->ifnum >= 0; ++quirk) {
		iface = usb_ifnum_to_if(chip->dev, quirk->ifnum);
		if (!iface)
			continue;
		if (quirk->ifnum != probed_ifnum &&
		    usb_interface_claimed(iface))
			continue;
		err = snd_usb_create_quirk(chip, iface, driver, quirk);
		if (err < 0)
			return err;
	}

	for (quirk = quirk_comp->data; quirk->ifnum >= 0; ++quirk) {
		iface = usb_ifnum_to_if(chip->dev, quirk->ifnum);
		if (!iface)
			continue;
		if (quirk->ifnum != probed_ifnum &&
		    !usb_interface_claimed(iface))
			usb_driver_claim_interface(driver, iface, (void *)-1L);
	}

	return 0;
}