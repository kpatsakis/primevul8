get_target_base(struct timer_base *base, unsigned tflags)
{
#ifdef CONFIG_SMP
	if ((tflags & TIMER_PINNED) || !base->migration_enabled)
		return get_timer_this_cpu_base(tflags);
	return get_timer_cpu_base(tflags, get_nohz_timer_target());
#else
	return get_timer_this_cpu_base(tflags);
#endif
}