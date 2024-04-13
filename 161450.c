int __sched schedule_hrtimeout(ktime_t *expires,
			       const enum hrtimer_mode mode)
{
	return schedule_hrtimeout_range(expires, 0, mode);
}