static struct snd_timer_instance *snd_timer_instance_new(char *owner,
							 struct snd_timer *timer)
{
	struct snd_timer_instance *timeri;
	timeri = kzalloc(sizeof(*timeri), GFP_KERNEL);
	if (timeri == NULL)
		return NULL;
	timeri->owner = kstrdup(owner, GFP_KERNEL);
	if (! timeri->owner) {
		kfree(timeri);
		return NULL;
	}
	INIT_LIST_HEAD(&timeri->open_list);
	INIT_LIST_HEAD(&timeri->active_list);
	INIT_LIST_HEAD(&timeri->ack_list);
	INIT_LIST_HEAD(&timeri->slave_list_head);
	INIT_LIST_HEAD(&timeri->slave_active_head);

	timeri->timer = timer;
	if (timer && !try_module_get(timer->module)) {
		kfree(timeri->owner);
		kfree(timeri);
		return NULL;
	}

	return timeri;
}