static int usb_chmap_ctl_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);
	struct snd_usb_substream *subs = info->private_data;
	struct snd_pcm_chmap_elem *chmap = NULL;
	int i;

	memset(ucontrol->value.integer.value, 0,
	       sizeof(ucontrol->value.integer.value));
	if (subs->cur_audiofmt)
		chmap = subs->cur_audiofmt->chmap;
	if (chmap) {
		for (i = 0; i < chmap->channels; i++)
			ucontrol->value.integer.value[i] = chmap->map[i];
	}
	return 0;
}