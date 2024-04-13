static inline void debug_init(struct timer_list *timer)
{
	debug_timer_init(timer);
	trace_timer_init(timer);
}