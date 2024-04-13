static void snd_timer_free_all(void)
{
	struct snd_timer *timer, *n;

	list_for_each_entry_safe(timer, n, &snd_timer_list, device_list)
		snd_timer_free(timer);
}