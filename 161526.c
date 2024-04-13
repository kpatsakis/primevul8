struct hrtimer_cpu_base *get_target_base(struct hrtimer_cpu_base *base,
					 int pinned)
{
	if (pinned || !base->migration_enabled)
		return base;
	return &per_cpu(hrtimer_bases, get_nohz_timer_target());
}