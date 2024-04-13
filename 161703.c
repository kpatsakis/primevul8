static inline ktime_t hrtimer_update_lowres(struct hrtimer *timer, ktime_t tim,
					    const enum hrtimer_mode mode)
{
#ifdef CONFIG_TIME_LOW_RES
	/*
	 * CONFIG_TIME_LOW_RES indicates that the system has no way to return
	 * granular time values. For relative timers we add hrtimer_resolution
	 * (i.e. one jiffie) to prevent short timeouts.
	 */
	timer->is_rel = mode & HRTIMER_MODE_REL;
	if (timer->is_rel)
		tim = ktime_add_safe(tim, hrtimer_resolution);
#endif
	return tim;
}