static inline int collect_expired_timers(struct timer_base *base,
					 struct hlist_head *heads)
{
	return __collect_expired_timers(base, heads);
}