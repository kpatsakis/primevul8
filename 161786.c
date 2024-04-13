void __sched usleep_range(unsigned long min, unsigned long max)
{
	ktime_t exp = ktime_add_us(ktime_get(), min);
	u64 delta = (u64)(max - min) * NSEC_PER_USEC;

	for (;;) {
		__set_current_state(TASK_UNINTERRUPTIBLE);
		/* Do not return before the requested sleep time has elapsed */
		if (!schedule_hrtimeout_range(&exp, delta, HRTIMER_MODE_ABS))
			break;
	}
}