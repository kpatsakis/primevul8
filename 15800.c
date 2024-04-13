static int snd_usb_audio_create(struct usb_interface *intf,
				struct usb_device *dev, int idx,
				const struct snd_usb_audio_quirk *quirk,
				unsigned int usb_id,
				struct snd_usb_audio **rchip)
{
	struct snd_card *card;
	struct snd_usb_audio *chip;
	int err;
	char component[14];

	*rchip = NULL;

	switch (snd_usb_get_speed(dev)) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL:
	case USB_SPEED_HIGH:
	case USB_SPEED_WIRELESS:
	case USB_SPEED_SUPER:
	case USB_SPEED_SUPER_PLUS:
		break;
	default:
		dev_err(&dev->dev, "unknown device speed %d\n", snd_usb_get_speed(dev));
		return -ENXIO;
	}

	err = snd_card_new(&intf->dev, index[idx], id[idx], THIS_MODULE,
			   sizeof(*chip), &card);
	if (err < 0) {
		dev_err(&dev->dev, "cannot create card instance %d\n", idx);
		return err;
	}

	chip = card->private_data;
	mutex_init(&chip->mutex);
	init_waitqueue_head(&chip->shutdown_wait);
	chip->index = idx;
	chip->dev = dev;
	chip->card = card;
	chip->setup = device_setup[idx];
	chip->autoclock = autoclock;
	atomic_set(&chip->active, 1); /* avoid autopm during probing */
	atomic_set(&chip->usage_count, 0);
	atomic_set(&chip->shutdown, 0);

	chip->usb_id = usb_id;
	INIT_LIST_HEAD(&chip->pcm_list);
	INIT_LIST_HEAD(&chip->ep_list);
	INIT_LIST_HEAD(&chip->midi_list);
	INIT_LIST_HEAD(&chip->mixer_list);

	card->private_free = snd_usb_audio_free;

	strcpy(card->driver, "USB-Audio");
	sprintf(component, "USB%04x:%04x",
		USB_ID_VENDOR(chip->usb_id), USB_ID_PRODUCT(chip->usb_id));
	snd_component_add(card, component);

	usb_audio_make_shortname(dev, chip, quirk);
	usb_audio_make_longname(dev, chip, quirk);

	snd_usb_audio_create_proc(chip);

	*rchip = chip;
	return 0;
}