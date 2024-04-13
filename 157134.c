static int create_roland_midi_quirk(struct snd_usb_audio *chip,
				    struct usb_interface *iface,
				    struct usb_driver *driver,
				    struct usb_host_interface *alts)
{
	static const struct snd_usb_audio_quirk roland_midi_quirk = {
		.type = QUIRK_MIDI_ROLAND
	};
	u8 *roland_desc = NULL;

	/* might have a vendor-specific descriptor <06 24 F1 02 ...> */
	for (;;) {
		roland_desc = snd_usb_find_csint_desc(alts->extra,
						      alts->extralen,
						      roland_desc, 0xf1);
		if (!roland_desc)
			return -ENODEV;
		if (roland_desc[0] < 6 || roland_desc[3] != 2)
			continue;
		return create_any_midi_quirk(chip, iface, driver,
					     &roland_midi_quirk);
	}
}