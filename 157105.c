int snd_usb_apply_boot_quirk(struct usb_device *dev,
			     struct usb_interface *intf,
			     const struct snd_usb_audio_quirk *quirk,
			     unsigned int id)
{
	switch (id) {
	case USB_ID(0x041e, 0x3000):
		/* SB Extigy needs special boot-up sequence */
		/* if more models come, this will go to the quirk list. */
		return snd_usb_extigy_boot_quirk(dev, intf);

	case USB_ID(0x041e, 0x3020):
		/* SB Audigy 2 NX needs its own boot-up magic, too */
		return snd_usb_audigy2nx_boot_quirk(dev);

	case USB_ID(0x10f5, 0x0200):
		/* C-Media CM106 / Turtle Beach Audio Advantage Roadie */
		return snd_usb_cm106_boot_quirk(dev);

	case USB_ID(0x0d8c, 0x0102):
		/* C-Media CM6206 / CM106-Like Sound Device */
	case USB_ID(0x0ccd, 0x00b1): /* Terratec Aureon 7.1 USB */
		return snd_usb_cm6206_boot_quirk(dev);

	case USB_ID(0x0dba, 0x3000):
		/* Digidesign Mbox 2 */
		return snd_usb_mbox2_boot_quirk(dev);

	case USB_ID(0x1235, 0x0010): /* Focusrite Novation Saffire 6 USB */
	case USB_ID(0x1235, 0x0018): /* Focusrite Novation Twitch */
		return snd_usb_novation_boot_quirk(dev);

	case USB_ID(0x133e, 0x0815):
		/* Access Music VirusTI Desktop */
		return snd_usb_accessmusic_boot_quirk(dev);

	case USB_ID(0x17cc, 0x1000): /* Komplete Audio 6 */
	case USB_ID(0x17cc, 0x1010): /* Traktor Audio 6 */
	case USB_ID(0x17cc, 0x1020): /* Traktor Audio 10 */
		return snd_usb_nativeinstruments_boot_quirk(dev);
	case USB_ID(0x0763, 0x2012):  /* M-Audio Fast Track Pro USB */
		return snd_usb_fasttrackpro_boot_quirk(dev);
	case USB_ID(0x047f, 0xc010): /* Plantronics Gamecom 780 */
		return snd_usb_gamecon780_boot_quirk(dev);
	}

	return 0;
}