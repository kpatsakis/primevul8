static inline void debug_deactivate(struct hrtimer *timer)
{
	debug_hrtimer_deactivate(timer);
	trace_hrtimer_cancel(timer);
}