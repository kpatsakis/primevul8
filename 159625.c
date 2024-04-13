static void __exit snd_timer_proc_done(void)
{
	snd_info_free_entry(snd_timer_proc_entry);
}