int snd_usb_create_quirk(struct snd_usb_audio *chip,
			 struct usb_interface *iface,
			 struct usb_driver *driver,
			 const struct snd_usb_audio_quirk *quirk)
{
	typedef int (*quirk_func_t)(struct snd_usb_audio *,
				    struct usb_interface *,
				    struct usb_driver *,
				    const struct snd_usb_audio_quirk *);
	static const quirk_func_t quirk_funcs[] = {
		[QUIRK_IGNORE_INTERFACE] = ignore_interface_quirk,
		[QUIRK_COMPOSITE] = create_composite_quirk,
		[QUIRK_AUTODETECT] = create_autodetect_quirks,
		[QUIRK_MIDI_STANDARD_INTERFACE] = create_any_midi_quirk,
		[QUIRK_MIDI_FIXED_ENDPOINT] = create_any_midi_quirk,
		[QUIRK_MIDI_YAMAHA] = create_any_midi_quirk,
		[QUIRK_MIDI_ROLAND] = create_any_midi_quirk,
		[QUIRK_MIDI_MIDIMAN] = create_any_midi_quirk,
		[QUIRK_MIDI_NOVATION] = create_any_midi_quirk,
		[QUIRK_MIDI_RAW_BYTES] = create_any_midi_quirk,
		[QUIRK_MIDI_EMAGIC] = create_any_midi_quirk,
		[QUIRK_MIDI_CME] = create_any_midi_quirk,
		[QUIRK_MIDI_AKAI] = create_any_midi_quirk,
		[QUIRK_MIDI_FTDI] = create_any_midi_quirk,
		[QUIRK_MIDI_CH345] = create_any_midi_quirk,
		[QUIRK_AUDIO_STANDARD_INTERFACE] = create_standard_audio_quirk,
		[QUIRK_AUDIO_FIXED_ENDPOINT] = create_fixed_stream_quirk,
		[QUIRK_AUDIO_EDIROL_UAXX] = create_uaxx_quirk,
		[QUIRK_AUDIO_ALIGN_TRANSFER] = create_align_transfer_quirk,
		[QUIRK_AUDIO_STANDARD_MIXER] = create_standard_mixer_quirk,
	};

	if (quirk->type < QUIRK_TYPE_COUNT) {
		return quirk_funcs[quirk->type](chip, iface, driver, quirk);
	} else {
		usb_audio_err(chip, "invalid quirk type %d\n", quirk->type);
		return -ENXIO;
	}
}