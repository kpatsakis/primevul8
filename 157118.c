static int snd_usb_accessmusic_boot_quirk(struct usb_device *dev)
{
	int err, actual_length;

	/* "midi send" enable */
	static const u8 seq[] = { 0x4e, 0x73, 0x52, 0x01 };

	void *buf = kmemdup(seq, ARRAY_SIZE(seq), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	err = usb_interrupt_msg(dev, usb_sndintpipe(dev, 0x05), buf,
			ARRAY_SIZE(seq), &actual_length, 1000);
	kfree(buf);
	if (err < 0)
		return err;

	return 0;
}