static void set_format_emu_quirk(struct snd_usb_substream *subs,
				 struct audioformat *fmt)
{
	unsigned char emu_samplerate_id = 0;

	/* When capture is active
	 * sample rate shouldn't be changed
	 * by playback substream
	 */
	if (subs->direction == SNDRV_PCM_STREAM_PLAYBACK) {
		if (subs->stream->substream[SNDRV_PCM_STREAM_CAPTURE].interface != -1)
			return;
	}

	switch (fmt->rate_min) {
	case 48000:
		emu_samplerate_id = EMU_QUIRK_SR_48000HZ;
		break;
	case 88200:
		emu_samplerate_id = EMU_QUIRK_SR_88200HZ;
		break;
	case 96000:
		emu_samplerate_id = EMU_QUIRK_SR_96000HZ;
		break;
	case 176400:
		emu_samplerate_id = EMU_QUIRK_SR_176400HZ;
		break;
	case 192000:
		emu_samplerate_id = EMU_QUIRK_SR_192000HZ;
		break;
	default:
		emu_samplerate_id = EMU_QUIRK_SR_44100HZ;
		break;
	}
	snd_emuusb_set_samplerate(subs->stream->chip, emu_samplerate_id);
	subs->pkt_offset_adj = (emu_samplerate_id >= EMU_QUIRK_SR_176400HZ) ? 4 : 0;
}