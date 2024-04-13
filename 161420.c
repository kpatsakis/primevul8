debug_activate(struct timer_list *timer, unsigned long expires)
{
	debug_timer_activate(timer);
	trace_timer_start(timer, expires, timer->flags);
}