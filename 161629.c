__mod_timer(struct timer_list *timer, unsigned long expires, bool pending_only)
{
	struct timer_base *base, *new_base;
	unsigned int idx = UINT_MAX;
	unsigned long clk = 0, flags;
	int ret = 0;

	BUG_ON(!timer->function);

	/*
	 * This is a common optimization triggered by the networking code - if
	 * the timer is re-modified to have the same timeout or ends up in the
	 * same array bucket then just return:
	 */
	if (timer_pending(timer)) {
		if (timer->expires == expires)
			return 1;

		/*
		 * We lock timer base and calculate the bucket index right
		 * here. If the timer ends up in the same bucket, then we
		 * just update the expiry time and avoid the whole
		 * dequeue/enqueue dance.
		 */
		base = lock_timer_base(timer, &flags);

		clk = base->clk;
		idx = calc_wheel_index(expires, clk);

		/*
		 * Retrieve and compare the array index of the pending
		 * timer. If it matches set the expiry to the new value so a
		 * subsequent call will exit in the expires check above.
		 */
		if (idx == timer_get_idx(timer)) {
			timer->expires = expires;
			ret = 1;
			goto out_unlock;
		}
	} else {
		base = lock_timer_base(timer, &flags);
	}

	ret = detach_if_pending(timer, base, false);
	if (!ret && pending_only)
		goto out_unlock;

	debug_activate(timer, expires);

	new_base = get_target_base(base, timer->flags);

	if (base != new_base) {
		/*
		 * We are trying to schedule the timer on the new base.
		 * However we can't change timer's base while it is running,
		 * otherwise del_timer_sync() can't detect that the timer's
		 * handler yet has not finished. This also guarantees that the
		 * timer is serialized wrt itself.
		 */
		if (likely(base->running_timer != timer)) {
			/* See the comment in lock_timer_base() */
			timer->flags |= TIMER_MIGRATING;

			spin_unlock(&base->lock);
			base = new_base;
			spin_lock(&base->lock);
			WRITE_ONCE(timer->flags,
				   (timer->flags & ~TIMER_BASEMASK) | base->cpu);
		}
	}

	/* Try to forward a stale timer base clock */
	forward_timer_base(base);

	timer->expires = expires;
	/*
	 * If 'idx' was calculated above and the base time did not advance
	 * between calculating 'idx' and possibly switching the base, only
	 * enqueue_timer() and trigger_dyntick_cpu() is required. Otherwise
	 * we need to (re)calculate the wheel index via
	 * internal_add_timer().
	 */
	if (idx != UINT_MAX && clk == base->clk) {
		enqueue_timer(base, timer, idx);
		trigger_dyntick_cpu(base, timer);
	} else {
		internal_add_timer(base, timer);
	}

out_unlock:
	spin_unlock_irqrestore(&base->lock, flags);

	return ret;
}