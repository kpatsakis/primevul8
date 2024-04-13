void snd_usb_set_format_quirk(struct snd_usb_substream *subs,
			      struct audioformat *fmt)
{
	switch (subs->stream->chip->usb_id) {
	case USB_ID(0x041e, 0x3f02): /* E-Mu 0202 USB */
	case USB_ID(0x041e, 0x3f04): /* E-Mu 0404 USB */
	case USB_ID(0x041e, 0x3f0a): /* E-Mu Tracker Pre */
	case USB_ID(0x041e, 0x3f19): /* E-Mu 0204 USB */
		set_format_emu_quirk(subs, fmt);
		break;
	}
}