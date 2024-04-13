int hrtimer_try_to_cancel(struct hrtimer *timer)
{
	struct hrtimer_clock_base *base;
	unsigned long flags;
	int ret = -1;

	/*
	 * Check lockless first. If the timer is not active (neither
	 * enqueued nor running the callback, nothing to do here.  The
	 * base lock does not serialize against a concurrent enqueue,
	 * so we can avoid taking it.
	 */
	if (!hrtimer_active(timer))
		return 0;

	base = lock_hrtimer_base(timer, &flags);

	if (!hrtimer_callback_running(timer))
		ret = remove_hrtimer(timer, base, false);

	unlock_hrtimer_base(timer, &flags);

	return ret;

}