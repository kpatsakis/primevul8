void timers_update_migration(bool update_nohz)
{
	bool on = sysctl_timer_migration && tick_nohz_active;
	unsigned int cpu;

	/* Avoid the loop, if nothing to update */
	if (this_cpu_read(timer_bases[BASE_STD].migration_enabled) == on)
		return;

	for_each_possible_cpu(cpu) {
		per_cpu(timer_bases[BASE_STD].migration_enabled, cpu) = on;
		per_cpu(timer_bases[BASE_DEF].migration_enabled, cpu) = on;
		per_cpu(hrtimer_bases.migration_enabled, cpu) = on;
		if (!update_nohz)
			continue;
		per_cpu(timer_bases[BASE_STD].nohz_active, cpu) = true;
		per_cpu(timer_bases[BASE_DEF].nohz_active, cpu) = true;
		per_cpu(hrtimer_bases.nohz_active, cpu) = true;
	}
}