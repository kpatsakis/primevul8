__internal_add_timer(struct timer_base *base, struct timer_list *timer)
{
	unsigned int idx;

	idx = calc_wheel_index(timer->expires, base->clk);
	enqueue_timer(base, timer, idx);
}