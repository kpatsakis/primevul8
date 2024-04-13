switch_hrtimer_base(struct hrtimer *timer, struct hrtimer_clock_base *base,
		    int pinned)
{
	struct hrtimer_cpu_base *new_cpu_base, *this_cpu_base;
	struct hrtimer_clock_base *new_base;
	int basenum = base->index;

	this_cpu_base = this_cpu_ptr(&hrtimer_bases);
	new_cpu_base = get_target_base(this_cpu_base, pinned);
again:
	new_base = &new_cpu_base->clock_base[basenum];

	if (base != new_base) {
		/*
		 * We are trying to move timer to new_base.
		 * However we can't change timer's base while it is running,
		 * so we keep it on the same CPU. No hassle vs. reprogramming
		 * the event source in the high resolution case. The softirq
		 * code will take care of this when the timer function has
		 * completed. There is no conflict as we hold the lock until
		 * the timer is enqueued.
		 */
		if (unlikely(hrtimer_callback_running(timer)))
			return base;

		/* See the comment in lock_hrtimer_base() */
		timer->base = &migration_base;
		raw_spin_unlock(&base->cpu_base->lock);
		raw_spin_lock(&new_base->cpu_base->lock);

		if (new_cpu_base != this_cpu_base &&
		    hrtimer_check_target(timer, new_base)) {
			raw_spin_unlock(&new_base->cpu_base->lock);
			raw_spin_lock(&base->cpu_base->lock);
			new_cpu_base = this_cpu_base;
			timer->base = base;
			goto again;
		}
		timer->base = new_base;
	} else {
		if (new_cpu_base != this_cpu_base &&
		    hrtimer_check_target(timer, new_base)) {
			new_cpu_base = this_cpu_base;
			goto again;
		}
	}
	return new_base;
}