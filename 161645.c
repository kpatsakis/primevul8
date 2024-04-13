int __sched schedule_hrtimeout_range(ktime_t *expires, u64 delta,
				     const enum hrtimer_mode mode)
{
	return schedule_hrtimeout_range_clock(expires, delta, mode,
					      CLOCK_MONOTONIC);
}