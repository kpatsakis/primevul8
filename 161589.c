static inline void forward_timer_base(struct timer_base *base)
{
	unsigned long jnow = READ_ONCE(jiffies);

	/*
	 * We only forward the base when it's idle and we have a delta between
	 * base clock and jiffies.
	 */
	if (!base->is_idle || (long) (jnow - base->clk) < 2)
		return;

	/*
	 * If the next expiry value is > jiffies, then we fast forward to
	 * jiffies otherwise we forward to the next expiry value.
	 */
	if (time_after(base->next_expiry, jnow))
		base->clk = jnow;
	else
		base->clk = base->next_expiry;
}