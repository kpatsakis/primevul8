static int create_any_midi_quirk(struct snd_usb_audio *chip,
				 struct usb_interface *intf,
				 struct usb_driver *driver,
				 const struct snd_usb_audio_quirk *quirk)
{
	return snd_usbmidi_create(chip->card, intf, &chip->midi_list, quirk);
}