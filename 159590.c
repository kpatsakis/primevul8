unsigned long snd_timer_resolution(struct snd_timer_instance *timeri)
{
	struct snd_timer * timer;

	if (timeri == NULL)
		return 0;
	if ((timer = timeri->timer) != NULL) {
		if (timer->hw.c_resolution)
			return timer->hw.c_resolution(timer);
		return timer->hw.resolution;
	}
	return 0;
}