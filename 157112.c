static int create_standard_mixer_quirk(struct snd_usb_audio *chip,
				       struct usb_interface *iface,
				       struct usb_driver *driver,
				       const struct snd_usb_audio_quirk *quirk)
{
	if (quirk->ifnum < 0)
		return 0;

	return snd_usb_create_mixer(chip, quirk->ifnum, 0);
}