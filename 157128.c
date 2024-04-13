static int create_autodetect_quirk(struct snd_usb_audio *chip,
				   struct usb_interface *iface,
				   struct usb_driver *driver)
{
	int err;

	err = create_auto_pcm_quirk(chip, iface, driver);
	if (err == -ENODEV)
		err = create_auto_midi_quirk(chip, iface, driver);
	return err;
}