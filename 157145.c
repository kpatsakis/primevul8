int snd_usb_apply_interface_quirk(struct snd_usb_audio *chip,
				  int iface,
				  int altno)
{
	/* audiophile usb: skip altsets incompatible with device_setup */
	if (chip->usb_id == USB_ID(0x0763, 0x2003))
		return audiophile_skip_setting_quirk(chip, iface, altno);
	/* quattro usb: skip altsets incompatible with device_setup */
	if (chip->usb_id == USB_ID(0x0763, 0x2001))
		return quattro_skip_setting_quirk(chip, iface, altno);
	/* fasttrackpro usb: skip altsets incompatible with device_setup */
	if (chip->usb_id == USB_ID(0x0763, 0x2012))
		return fasttrackpro_skip_setting_quirk(chip, iface, altno);

	return 0;
}