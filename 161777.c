static inline void __hrtimer_peek_ahead_timers(void)
{
	struct tick_device *td;

	if (!hrtimer_hres_active())
		return;

	td = this_cpu_ptr(&tick_cpu_device);
	if (td && td->evtdev)
		hrtimer_interrupt(td->evtdev);
}