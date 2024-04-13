static inline void debug_timer_assert_init(struct timer_list *timer)
{
	debug_object_assert_init(timer, &timer_debug_descr);
}