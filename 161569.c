static inline void __run_timers(struct timer_base *base)
{
	struct hlist_head heads[LVL_DEPTH];
	int levels;

	if (!time_after_eq(jiffies, base->clk))
		return;

	spin_lock_irq(&base->lock);

	while (time_after_eq(jiffies, base->clk)) {

		levels = collect_expired_timers(base, heads);
		base->clk++;

		while (levels--)
			expire_timers(base, heads + levels);
	}
	base->running_timer = NULL;
	spin_unlock_irq(&base->lock);
}