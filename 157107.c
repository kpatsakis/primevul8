static int create_auto_pcm_quirk(struct snd_usb_audio *chip,
				 struct usb_interface *iface,
				 struct usb_driver *driver)
{
	struct usb_host_interface *alts;
	struct usb_interface_descriptor *altsd;
	struct usb_endpoint_descriptor *epd;
	struct uac1_as_header_descriptor *ashd;
	struct uac_format_type_i_discrete_descriptor *fmtd;

	/*
	 * Most Roland/Yamaha audio streaming interfaces have more or less
	 * standard descriptors, but older devices might lack descriptors, and
	 * future ones might change, so ensure that we fail silently if the
	 * interface doesn't look exactly right.
	 */

	/* must have a non-zero altsetting for streaming */
	if (iface->num_altsetting < 2)
		return -ENODEV;
	alts = &iface->altsetting[1];
	altsd = get_iface_desc(alts);

	/* must have an isochronous endpoint for streaming */
	if (altsd->bNumEndpoints < 1)
		return -ENODEV;
	epd = get_endpoint(alts, 0);
	if (!usb_endpoint_xfer_isoc(epd))
		return -ENODEV;

	/* must have format descriptors */
	ashd = snd_usb_find_csint_desc(alts->extra, alts->extralen, NULL,
				       UAC_AS_GENERAL);
	fmtd = snd_usb_find_csint_desc(alts->extra, alts->extralen, NULL,
				       UAC_FORMAT_TYPE);
	if (!ashd || ashd->bLength < 7 ||
	    !fmtd || fmtd->bLength < 8)
		return -ENODEV;

	return create_standard_audio_quirk(chip, iface, driver, NULL);
}