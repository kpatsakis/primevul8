static int snd_timer_user_info(struct file *file,
			       struct snd_timer_info __user *_info)
{
	struct snd_timer_user *tu;
	struct snd_timer_info *info;
	struct snd_timer *t;
	int err = 0;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	t = tu->timeri->timer;
	if (!t)
		return -EBADFD;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (! info)
		return -ENOMEM;
	info->card = t->card ? t->card->number : -1;
	if (t->hw.flags & SNDRV_TIMER_HW_SLAVE)
		info->flags |= SNDRV_TIMER_FLG_SLAVE;
	strlcpy(info->id, t->id, sizeof(info->id));
	strlcpy(info->name, t->name, sizeof(info->name));
	info->resolution = t->hw.resolution;
	if (copy_to_user(_info, info, sizeof(*_info)))
		err = -EFAULT;
	kfree(info);
	return err;
}