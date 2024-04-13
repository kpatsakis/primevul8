int snd_timer_global_register(struct snd_timer *timer)
{
	struct snd_device dev;

	memset(&dev, 0, sizeof(dev));
	dev.device_data = timer;
	return snd_timer_dev_register(&dev);
}