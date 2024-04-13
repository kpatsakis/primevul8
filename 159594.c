static struct snd_timer *snd_timer_find(struct snd_timer_id *tid)
{
	struct snd_timer *timer = NULL;

	list_for_each_entry(timer, &snd_timer_list, device_list) {
		if (timer->tmr_class != tid->dev_class)
			continue;
		if ((timer->tmr_class == SNDRV_TIMER_CLASS_CARD ||
		     timer->tmr_class == SNDRV_TIMER_CLASS_PCM) &&
		    (timer->card == NULL ||
		     timer->card->number != tid->card))
			continue;
		if (timer->tmr_device != tid->device)
			continue;
		if (timer->tmr_subdevice != tid->subdevice)
			continue;
		return timer;
	}
	return NULL;
}