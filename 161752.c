static inline void timer_set_idx(struct timer_list *timer, unsigned int idx)
{
	timer->flags = (timer->flags & ~TIMER_ARRAYMASK) |
			idx << TIMER_ARRAYSHIFT;
}