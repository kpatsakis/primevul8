static inline unsigned int timer_get_idx(struct timer_list *timer)
{
	return (timer->flags & TIMER_ARRAYMASK) >> TIMER_ARRAYSHIFT;
}