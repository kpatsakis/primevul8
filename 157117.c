static int create_yamaha_midi_quirk(struct snd_usb_audio *chip,
				    struct usb_interface *iface,
				    struct usb_driver *driver,
				    struct usb_host_interface *alts)
{
	static const struct snd_usb_audio_quirk yamaha_midi_quirk = {
		.type = QUIRK_MIDI_YAMAHA
	};
	struct usb_midi_in_jack_descriptor *injd;
	struct usb_midi_out_jack_descriptor *outjd;

	/* must have some valid jack descriptors */
	injd = snd_usb_find_csint_desc(alts->extra, alts->extralen,
				       NULL, USB_MS_MIDI_IN_JACK);
	outjd = snd_usb_find_csint_desc(alts->extra, alts->extralen,
					NULL, USB_MS_MIDI_OUT_JACK);
	if (!injd && !outjd)
		return -ENODEV;
	if (injd && (injd->bLength < 5 ||
		     (injd->bJackType != USB_MS_EMBEDDED &&
		      injd->bJackType != USB_MS_EXTERNAL)))
		return -ENODEV;
	if (outjd && (outjd->bLength < 6 ||
		      (outjd->bJackType != USB_MS_EMBEDDED &&
		       outjd->bJackType != USB_MS_EXTERNAL)))
		return -ENODEV;
	return create_any_midi_quirk(chip, iface, driver, &yamaha_midi_quirk);
}