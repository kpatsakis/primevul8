int snd_usb_add_audio_stream(struct snd_usb_audio *chip,
			     int stream,
			     struct audioformat *fp)
{
	struct snd_usb_stream *as;
	struct snd_usb_substream *subs;
	struct snd_pcm *pcm;
	int err;

	list_for_each_entry(as, &chip->pcm_list, list) {
		if (as->fmt_type != fp->fmt_type)
			continue;
		subs = &as->substream[stream];
		if (subs->ep_num == fp->endpoint) {
			list_add_tail(&fp->list, &subs->fmt_list);
			subs->num_formats++;
			subs->formats |= fp->formats;
			return 0;
		}
	}
	/* look for an empty stream */
	list_for_each_entry(as, &chip->pcm_list, list) {
		if (as->fmt_type != fp->fmt_type)
			continue;
		subs = &as->substream[stream];
		if (subs->ep_num)
			continue;
		err = snd_pcm_new_stream(as->pcm, stream, 1);
		if (err < 0)
			return err;
		snd_usb_init_substream(as, stream, fp);
		return add_chmap(as->pcm, stream, subs);
	}

	/* create a new pcm */
	as = kzalloc(sizeof(*as), GFP_KERNEL);
	if (!as)
		return -ENOMEM;
	as->pcm_index = chip->pcm_devs;
	as->chip = chip;
	as->fmt_type = fp->fmt_type;
	err = snd_pcm_new(chip->card, "USB Audio", chip->pcm_devs,
			  stream == SNDRV_PCM_STREAM_PLAYBACK ? 1 : 0,
			  stream == SNDRV_PCM_STREAM_PLAYBACK ? 0 : 1,
			  &pcm);
	if (err < 0) {
		kfree(as);
		return err;
	}
	as->pcm = pcm;
	pcm->private_data = as;
	pcm->private_free = snd_usb_audio_pcm_free;
	pcm->info_flags = 0;
	if (chip->pcm_devs > 0)
		sprintf(pcm->name, "USB Audio #%d", chip->pcm_devs);
	else
		strcpy(pcm->name, "USB Audio");

	snd_usb_init_substream(as, stream, fp);

	/*
	 * Keep using head insertion for M-Audio Audiophile USB (tm) which has a
	 * fix to swap capture stream order in conf/cards/USB-audio.conf
	 */
	if (chip->usb_id == USB_ID(0x0763, 0x2003))
		list_add(&as->list, &chip->pcm_list);
	else
		list_add_tail(&as->list, &chip->pcm_list);

	chip->pcm_devs++;

	snd_usb_proc_pcm_format_add(as);

	return add_chmap(pcm, stream, &as->substream[stream]);
}