static inline int timer_pending(const struct timer_list * timer)
{
	return timer->entry.pprev != NULL;
}