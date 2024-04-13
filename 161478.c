static inline void debug_timer_deactivate(struct timer_list *timer)
{
	debug_object_deactivate(timer, &timer_debug_descr);
}