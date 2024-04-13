static inline void debug_timer_init(struct timer_list *timer)
{
	debug_object_init(timer, &timer_debug_descr);
}