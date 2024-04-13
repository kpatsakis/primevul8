static void __init snd_timer_proc_init(void)
{
	struct snd_info_entry *entry;

	entry = snd_info_create_module_entry(THIS_MODULE, "timers", NULL);
	if (entry != NULL) {
		entry->c.text.read = snd_timer_proc_read;
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	snd_timer_proc_entry = entry;
}