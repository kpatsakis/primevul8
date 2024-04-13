static inline void debug_timer_activate(struct timer_list *timer)
{
	debug_object_activate(timer, &timer_debug_descr);
}