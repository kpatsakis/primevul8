void sysrq_timer_list_show(void)
{
	u64 now = ktime_to_ns(ktime_get());
	int cpu;

	timer_list_header(NULL, now);

	for_each_online_cpu(cpu)
		print_cpu(NULL, cpu, now);

#ifdef CONFIG_GENERIC_CLOCKEVENTS
	timer_list_show_tickdevices_header(NULL);
	for_each_online_cpu(cpu)
		print_tickdevice(NULL, tick_get_device(cpu), cpu);
#endif
	return;
}