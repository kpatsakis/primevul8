void snd_usb_endpoint_start_quirk(struct snd_usb_endpoint *ep)
{
	/*
	 * "Playback Design" products send bogus feedback data at the start
	 * of the stream. Ignore them.
	 */
	if (USB_ID_VENDOR(ep->chip->usb_id) == 0x23ba &&
	    ep->type == SND_USB_ENDPOINT_TYPE_SYNC)
		ep->skip_packets = 4;

	/*
	 * M-Audio Fast Track C400/C600 - when packets are not skipped, real
	 * world latency varies by approx. +/- 50 frames (at 96KHz) each time
	 * the stream is (re)started. When skipping packets 16 at endpoint
	 * start up, the real world latency is stable within +/- 1 frame (also
	 * across power cycles).
	 */
	if ((ep->chip->usb_id == USB_ID(0x0763, 0x2030) ||
	     ep->chip->usb_id == USB_ID(0x0763, 0x2031)) &&
	    ep->type == SND_USB_ENDPOINT_TYPE_DATA)
		ep->skip_packets = 16;
}