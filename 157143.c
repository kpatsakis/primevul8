static int add_chmap(struct snd_pcm *pcm, int stream,
		     struct snd_usb_substream *subs)
{
	struct audioformat *fp;
	struct snd_pcm_chmap *chmap;
	struct snd_kcontrol *kctl;
	int err;

	list_for_each_entry(fp, &subs->fmt_list, list)
		if (fp->chmap)
			goto ok;
	/* no chmap is found */
	return 0;

 ok:
	err = snd_pcm_add_chmap_ctls(pcm, stream, NULL, 0, 0, &chmap);
	if (err < 0)
		return err;

	/* override handlers */
	chmap->private_data = subs;
	kctl = chmap->kctl;
	kctl->info = usb_chmap_ctl_info;
	kctl->get = usb_chmap_ctl_get;
	kctl->tlv.c = usb_chmap_ctl_tlv;

	return 0;
}