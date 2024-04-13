void destroy_timer_on_stack(struct timer_list *timer)
{
	debug_object_free(timer, &timer_debug_descr);
}