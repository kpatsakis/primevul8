static int snd_timer_dev_register(struct snd_device *dev)
{
	struct snd_timer *timer = dev->device_data;
	struct snd_timer *timer1;

	if (snd_BUG_ON(!timer || !timer->hw.start || !timer->hw.stop))
		return -ENXIO;
	if (!(timer->hw.flags & SNDRV_TIMER_HW_SLAVE) &&
	    !timer->hw.resolution && timer->hw.c_resolution == NULL)
	    	return -EINVAL;

	mutex_lock(&register_mutex);
	list_for_each_entry(timer1, &snd_timer_list, device_list) {
		if (timer1->tmr_class > timer->tmr_class)
			break;
		if (timer1->tmr_class < timer->tmr_class)
			continue;
		if (timer1->card && timer->card) {
			if (timer1->card->number > timer->card->number)
				break;
			if (timer1->card->number < timer->card->number)
				continue;
		}
		if (timer1->tmr_device > timer->tmr_device)
			break;
		if (timer1->tmr_device < timer->tmr_device)
			continue;
		if (timer1->tmr_subdevice > timer->tmr_subdevice)
			break;
		if (timer1->tmr_subdevice < timer->tmr_subdevice)
			continue;
		/* conflicts.. */
		mutex_unlock(&register_mutex);
		return -EBUSY;
	}
	list_add_tail(&timer->device_list, &timer1->device_list);
	mutex_unlock(&register_mutex);
	return 0;
}