struct hrtimer_clock_base *lock_hrtimer_base(const struct hrtimer *timer,
					     unsigned long *flags)
{
	struct hrtimer_clock_base *base;

	for (;;) {
		base = timer->base;
		if (likely(base != &migration_base)) {
			raw_spin_lock_irqsave(&base->cpu_base->lock, *flags);
			if (likely(base == timer->base))
				return base;
			/* The timer has migrated to another CPU: */
			raw_spin_unlock_irqrestore(&base->cpu_base->lock, *flags);
		}
		cpu_relax();
	}
}