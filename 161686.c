static int detach_if_pending(struct timer_list *timer, struct timer_base *base,
			     bool clear_pending)
{
	unsigned idx = timer_get_idx(timer);

	if (!timer_pending(timer))
		return 0;

	if (hlist_is_singular_node(&timer->entry, base->vectors + idx))
		__clear_bit(idx, base->pending_map);

	detach_timer(timer, clear_pending);
	return 1;
}