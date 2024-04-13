static inline void debug_deactivate(struct timer_list *timer)
{
	debug_timer_deactivate(timer);
	trace_timer_cancel(timer);
}