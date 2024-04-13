static int collect_expired_timers(struct timer_base *base,
				  struct hlist_head *heads)
{
	/*
	 * NOHZ optimization. After a long idle sleep we need to forward the
	 * base to current jiffies. Avoid a loop by searching the bitfield for
	 * the next expiring timer.
	 */
	if ((long)(jiffies - base->clk) > 2) {
		unsigned long next = __next_timer_interrupt(base);

		/*
		 * If the next timer is ahead of time forward to current
		 * jiffies, otherwise forward to the next expiry time:
		 */
		if (time_after(next, jiffies)) {
			/* The call site will increment clock! */
			base->clk = jiffies - 1;
			return 0;
		}
		base->clk = next;
	}
	return __collect_expired_timers(base, heads);
}