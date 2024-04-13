static void __exit alsa_timer_exit(void)
{
	snd_unregister_device(&timer_dev);
	snd_timer_free_all();
	put_device(&timer_dev);
	snd_timer_proc_done();
#ifdef SNDRV_OSS_INFO_DEV_TIMERS
	snd_oss_info_unregister(SNDRV_OSS_INFO_DEV_TIMERS, SNDRV_CARDS - 1);
#endif
}