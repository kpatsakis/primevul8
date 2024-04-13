static int create_std_midi_quirk(struct snd_usb_audio *chip,
				 struct usb_interface *iface,
				 struct usb_driver *driver,
				 struct usb_host_interface *alts)
{
	struct usb_ms_header_descriptor *mshd;
	struct usb_ms_endpoint_descriptor *msepd;

	/* must have the MIDIStreaming interface header descriptor*/
	mshd = (struct usb_ms_header_descriptor *)alts->extra;
	if (alts->extralen < 7 ||
	    mshd->bLength < 7 ||
	    mshd->bDescriptorType != USB_DT_CS_INTERFACE ||
	    mshd->bDescriptorSubtype != USB_MS_HEADER)
		return -ENODEV;
	/* must have the MIDIStreaming endpoint descriptor*/
	msepd = (struct usb_ms_endpoint_descriptor *)alts->endpoint[0].extra;
	if (alts->endpoint[0].extralen < 4 ||
	    msepd->bLength < 4 ||
	    msepd->bDescriptorType != USB_DT_CS_ENDPOINT ||
	    msepd->bDescriptorSubtype != UAC_MS_GENERAL ||
	    msepd->bNumEmbMIDIJack < 1 ||
	    msepd->bNumEmbMIDIJack > 16)
		return -ENODEV;

	return create_any_midi_quirk(chip, iface, driver, NULL);
}