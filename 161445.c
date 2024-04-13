static inline void debug_timer_free(struct timer_list *timer)
{
	debug_object_free(timer, &timer_debug_descr);
}