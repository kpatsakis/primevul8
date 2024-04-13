static int timer_list_show(struct seq_file *m, void *v)
{
	struct timer_list_iter *iter = v;

	if (iter->cpu == -1 && !iter->second_pass)
		timer_list_header(m, iter->now);
	else if (!iter->second_pass)
		print_cpu(m, iter->cpu, iter->now);
#ifdef CONFIG_GENERIC_CLOCKEVENTS
	else if (iter->cpu == -1 && iter->second_pass)
		timer_list_show_tickdevices_header(m);
	else
		print_tickdevice(m, tick_get_device(iter->cpu), iter->cpu);
#endif
	return 0;
}