static void snd_usb_init_substream(struct snd_usb_stream *as,
				   int stream,
				   struct audioformat *fp)
{
	struct snd_usb_substream *subs = &as->substream[stream];

	INIT_LIST_HEAD(&subs->fmt_list);
	spin_lock_init(&subs->lock);

	subs->stream = as;
	subs->direction = stream;
	subs->dev = as->chip->dev;
	subs->txfr_quirk = as->chip->txfr_quirk;
	subs->tx_length_quirk = as->chip->tx_length_quirk;
	subs->speed = snd_usb_get_speed(subs->dev);
	subs->pkt_offset_adj = 0;

	snd_usb_set_pcm_ops(as->pcm, stream);

	list_add_tail(&fp->list, &subs->fmt_list);
	subs->formats |= fp->formats;
	subs->num_formats++;
	subs->fmt_type = fp->fmt_type;
	subs->ep_num = fp->endpoint;
	if (fp->channels > subs->channels_max)
		subs->channels_max = fp->channels;
}