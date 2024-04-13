static int parse_uac_endpoint_attributes(struct snd_usb_audio *chip,
					 struct usb_host_interface *alts,
					 int protocol, int iface_no)
{
	/* parsed with a v1 header here. that's ok as we only look at the
	 * header first which is the same for both versions */
	struct uac_iso_endpoint_descriptor *csep;
	struct usb_interface_descriptor *altsd = get_iface_desc(alts);
	int attributes = 0;

	csep = snd_usb_find_desc(alts->endpoint[0].extra, alts->endpoint[0].extralen, NULL, USB_DT_CS_ENDPOINT);

	/* Creamware Noah has this descriptor after the 2nd endpoint */
	if (!csep && altsd->bNumEndpoints >= 2)
		csep = snd_usb_find_desc(alts->endpoint[1].extra, alts->endpoint[1].extralen, NULL, USB_DT_CS_ENDPOINT);

	/*
	 * If we can't locate the USB_DT_CS_ENDPOINT descriptor in the extra
	 * bytes after the first endpoint, go search the entire interface.
	 * Some devices have it directly *before* the standard endpoint.
	 */
	if (!csep)
		csep = snd_usb_find_desc(alts->extra, alts->extralen, NULL, USB_DT_CS_ENDPOINT);

	if (!csep || csep->bLength < 7 ||
	    csep->bDescriptorSubtype != UAC_EP_GENERAL) {
		usb_audio_warn(chip,
			       "%u:%d : no or invalid class specific endpoint descriptor\n",
			       iface_no, altsd->bAlternateSetting);
		return 0;
	}

	if (protocol == UAC_VERSION_1) {
		attributes = csep->bmAttributes;
	} else {
		struct uac2_iso_endpoint_descriptor *csep2 =
			(struct uac2_iso_endpoint_descriptor *) csep;

		attributes = csep->bmAttributes & UAC_EP_CS_ATTR_FILL_MAX;

		/* emulate the endpoint attributes of a v1 device */
		if (csep2->bmControls & UAC2_CONTROL_PITCH)
			attributes |= UAC_EP_CS_ATTR_PITCH_CONTROL;
	}

	return attributes;
}