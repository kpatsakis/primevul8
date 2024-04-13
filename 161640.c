static inline void debug_activate(struct hrtimer *timer)
{
	debug_hrtimer_activate(timer);
	trace_hrtimer_start(timer);
}