static void snd_timer_free_system(struct snd_timer *timer)
{
	kfree(timer->private_data);
}