static int snd_timer_free(struct snd_timer *timer)
{
	if (!timer)
		return 0;

	mutex_lock(&register_mutex);
	if (! list_empty(&timer->open_list_head)) {
		struct list_head *p, *n;
		struct snd_timer_instance *ti;
		pr_warn("ALSA: timer %p is busy?\n", timer);
		list_for_each_safe(p, n, &timer->open_list_head) {
			list_del_init(p);
			ti = list_entry(p, struct snd_timer_instance, open_list);
			ti->timer = NULL;
		}
	}
	list_del(&timer->device_list);
	mutex_unlock(&register_mutex);

	if (timer->private_free)
		timer->private_free(timer);
	kfree(timer);
	return 0;
}