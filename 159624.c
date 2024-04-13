static int snd_timer_dev_free(struct snd_device *device)
{
	struct snd_timer *timer = device->device_data;
	return snd_timer_free(timer);
}