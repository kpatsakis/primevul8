static bool have_dup_chmap(struct snd_usb_substream *subs,
			   struct audioformat *fp)
{
	struct audioformat *prev = fp;

	list_for_each_entry_continue_reverse(prev, &subs->fmt_list, list) {
		if (prev->chmap &&
		    !memcmp(prev->chmap, fp->chmap, sizeof(*fp->chmap)))
			return true;
	}
	return false;
}