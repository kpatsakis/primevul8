static int usb_chmap_ctl_tlv(struct snd_kcontrol *kcontrol, int op_flag,
			     unsigned int size, unsigned int __user *tlv)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);
	struct snd_usb_substream *subs = info->private_data;
	struct audioformat *fp;
	unsigned int __user *dst;
	int count = 0;

	if (size < 8)
		return -ENOMEM;
	if (put_user(SNDRV_CTL_TLVT_CONTAINER, tlv))
		return -EFAULT;
	size -= 8;
	dst = tlv + 2;
	list_for_each_entry(fp, &subs->fmt_list, list) {
		int i, ch_bytes;

		if (!fp->chmap)
			continue;
		if (have_dup_chmap(subs, fp))
			continue;
		/* copy the entry */
		ch_bytes = fp->chmap->channels * 4;
		if (size < 8 + ch_bytes)
			return -ENOMEM;
		if (put_user(SNDRV_CTL_TLVT_CHMAP_FIXED, dst) ||
		    put_user(ch_bytes, dst + 1))
			return -EFAULT;
		dst += 2;
		for (i = 0; i < fp->chmap->channels; i++, dst++) {
			if (put_user(fp->chmap->map[i], dst))
				return -EFAULT;
		}

		count += 8 + ch_bytes;
		size -= 8 + ch_bytes;
	}
	if (put_user(count, tlv + 1))
		return -EFAULT;
	return 0;
}