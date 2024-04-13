static u64 cmp_next_hrtimer_event(u64 basem, u64 expires)
{
	u64 nextevt = hrtimer_get_next_event();

	/*
	 * If high resolution timers are enabled
	 * hrtimer_get_next_event() returns KTIME_MAX.
	 */
	if (expires <= nextevt)
		return expires;

	/*
	 * If the next timer is already expired, return the tick base
	 * time so the tick is fired immediately.
	 */
	if (nextevt <= basem)
		return basem;

	/*
	 * Round up to the next jiffie. High resolution timers are
	 * off, so the hrtimers are expired in the tick and we need to
	 * make sure that this tick really expires the timer to avoid
	 * a ping pong of the nohz stop code.
	 *
	 * Use DIV_ROUND_UP_ULL to prevent gcc calling __divdi3
	 */
	return DIV_ROUND_UP_ULL(nextevt, TICK_NSEC) * TICK_NSEC;
}