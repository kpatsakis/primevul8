u64 hrtimer_forward(struct hrtimer *timer, ktime_t now, ktime_t interval)
{
	u64 orun = 1;
	ktime_t delta;

	delta = ktime_sub(now, hrtimer_get_expires(timer));

	if (delta < 0)
		return 0;

	if (WARN_ON(timer->state & HRTIMER_STATE_ENQUEUED))
		return 0;

	if (interval < hrtimer_resolution)
		interval = hrtimer_resolution;

	if (unlikely(delta >= interval)) {
		s64 incr = ktime_to_ns(interval);

		orun = ktime_divns(delta, incr);
		hrtimer_add_expires_ns(timer, incr * orun);
		if (hrtimer_get_expires_tv64(timer) > now)
			return orun;
		/*
		 * This (and the ktime_add() below) is the
		 * correction for exact:
		 */
		orun++;
	}
	hrtimer_add_expires(timer, interval);

	return orun;
}