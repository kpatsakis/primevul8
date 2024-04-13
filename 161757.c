static inline struct timer_base *get_timer_cpu_base(u32 tflags, u32 cpu)
{
	struct timer_base *base = per_cpu_ptr(&timer_bases[BASE_STD], cpu);

	/*
	 * If the timer is deferrable and nohz is active then we need to use
	 * the deferrable base.
	 */
	if (IS_ENABLED(CONFIG_NO_HZ_COMMON) && base->nohz_active &&
	    (tflags & TIMER_DEFERRABLE))
		base = per_cpu_ptr(&timer_bases[BASE_DEF], cpu);
	return base;
}