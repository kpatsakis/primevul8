static void wq_watchdog_reset_touched(void)
{
	int cpu;

	wq_watchdog_touched = jiffies;
	for_each_possible_cpu(cpu)
		per_cpu(wq_watchdog_touched_cpu, cpu) = jiffies;
}