static int snd_timer_user_ginfo(struct file *file,
				struct snd_timer_ginfo __user *_ginfo)
{
	struct snd_timer_ginfo *ginfo;
	struct snd_timer_id tid;
	struct snd_timer *t;
	struct list_head *p;
	int err = 0;

	ginfo = memdup_user(_ginfo, sizeof(*ginfo));
	if (IS_ERR(ginfo))
		return PTR_ERR(ginfo);

	tid = ginfo->tid;
	memset(ginfo, 0, sizeof(*ginfo));
	ginfo->tid = tid;
	mutex_lock(&register_mutex);
	t = snd_timer_find(&tid);
	if (t != NULL) {
		ginfo->card = t->card ? t->card->number : -1;
		if (t->hw.flags & SNDRV_TIMER_HW_SLAVE)
			ginfo->flags |= SNDRV_TIMER_FLG_SLAVE;
		strlcpy(ginfo->id, t->id, sizeof(ginfo->id));
		strlcpy(ginfo->name, t->name, sizeof(ginfo->name));
		ginfo->resolution = t->hw.resolution;
		if (t->hw.resolution_min > 0) {
			ginfo->resolution_min = t->hw.resolution_min;
			ginfo->resolution_max = t->hw.resolution_max;
		}
		list_for_each(p, &t->open_list_head) {
			ginfo->clients++;
		}
	} else {
		err = -ENODEV;
	}
	mutex_unlock(&register_mutex);
	if (err >= 0 && copy_to_user(_ginfo, ginfo, sizeof(*ginfo)))
		err = -EFAULT;
	kfree(ginfo);
	return err;
}