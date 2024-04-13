static void free_substream(struct snd_usb_substream *subs)
{
	struct audioformat *fp, *n;

	if (!subs->num_formats)
		return; /* not initialized */
	list_for_each_entry_safe(fp, n, &subs->fmt_list, list) {
		kfree(fp->rate_table);
		kfree(fp->chmap);
		kfree(fp);
	}
	kfree(subs->rate_list.list);
}