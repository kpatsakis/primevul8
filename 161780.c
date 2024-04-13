internal_add_timer(struct timer_base *base, struct timer_list *timer)
{
	__internal_add_timer(base, timer);
	trigger_dyntick_cpu(base, timer);
}