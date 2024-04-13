static int audiophile_skip_setting_quirk(struct snd_usb_audio *chip,
					 int iface,
					 int altno)
{
	/* Reset ALL ifaces to 0 altsetting.
	 * Call it for every possible altsetting of every interface.
	 */
	usb_set_interface(chip->dev, iface, 0);

	if (chip->setup & MAUDIO_SET) {
		unsigned int mask;
		if ((chip->setup & MAUDIO_SET_DTS) && altno != 6)
			return 1; /* skip this altsetting */
		if ((chip->setup & MAUDIO_SET_96K) && altno != 1)
			return 1; /* skip this altsetting */
		mask = chip->setup & MAUDIO_SET_MASK;
		if (mask == MAUDIO_SET_24B_48K_DI && altno != 2)
			return 1; /* skip this altsetting */
		if (mask == MAUDIO_SET_24B_48K_NOTDI && altno != 3)
			return 1; /* skip this altsetting */
		if (mask == MAUDIO_SET_16B_48K_DI && altno != 4)
			return 1; /* skip this altsetting */
		if (mask == MAUDIO_SET_16B_48K_NOTDI && altno != 5)
			return 1; /* skip this altsetting */
	}

	return 0; /* keep this altsetting */
}