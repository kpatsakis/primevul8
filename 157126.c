static int snd_usb_audigy2nx_boot_quirk(struct usb_device *dev)
{
	u8 buf = 1;

	snd_usb_ctl_msg(dev, usb_rcvctrlpipe(dev, 0), 0x2a,
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_OTHER,
			0, 0, &buf, 1);
	if (buf == 0) {
		snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0), 0x29,
				USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_OTHER,
				1, 2000, NULL, 0);
		return -ENODEV;
	}
	return 0;
}