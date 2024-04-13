void wq_watchdog_touch(int cpu)
{
	if (cpu >= 0)
		per_cpu(wq_watchdog_touched_cpu, cpu) = jiffies;
	else
		wq_watchdog_touched = jiffies;
}