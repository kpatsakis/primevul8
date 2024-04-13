static int snd_usb_gamecon780_boot_quirk(struct usb_device *dev)
{
	/* set the initial volume and don't change; other values are either
	 * too loud or silent due to firmware bug (bko#65251)
	 */
	u8 buf[2] = { 0x74, 0xe3 };
	return snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0), UAC_SET_CUR,
			USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_OUT,
			UAC_FU_VOLUME << 8, 9 << 8, buf, 2);
}