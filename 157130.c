int snd_usb_is_big_endian_format(struct snd_usb_audio *chip, struct audioformat *fp)
{
	/* it depends on altsetting whether the device is big-endian or not */
	switch (chip->usb_id) {
	case USB_ID(0x0763, 0x2001): /* M-Audio Quattro: captured data only */
		if (fp->altsetting == 2 || fp->altsetting == 3 ||
			fp->altsetting == 5 || fp->altsetting == 6)
			return 1;
		break;
	case USB_ID(0x0763, 0x2003): /* M-Audio Audiophile USB */
		if (chip->setup == 0x00 ||
			fp->altsetting == 1 || fp->altsetting == 2 ||
			fp->altsetting == 3)
			return 1;
		break;
	case USB_ID(0x0763, 0x2012): /* M-Audio Fast Track Pro */
		if (fp->altsetting == 2 || fp->altsetting == 3 ||
			fp->altsetting == 5 || fp->altsetting == 6)
			return 1;
		break;
	}
	return 0;
}