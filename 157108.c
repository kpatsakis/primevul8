static void mbox2_setup_48_24_magic(struct usb_device *dev)
{
	u8 srate[3];
	u8 temp[12];

	/* Choose 48000Hz permanently */
	srate[0] = 0x80;
	srate[1] = 0xbb;
	srate[2] = 0x00;

	/* Send the magic! */
	snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0),
		0x01, 0x22, 0x0100, 0x0085, &temp, 0x0003);
	snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0),
		0x81, 0xa2, 0x0100, 0x0085, &srate, 0x0003);
	snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0),
		0x81, 0xa2, 0x0100, 0x0086, &srate, 0x0003);
	snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0),
		0x81, 0xa2, 0x0100, 0x0003, &srate, 0x0003);
	return;
}