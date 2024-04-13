int del_timer_sync(struct timer_list *timer)
{
#ifdef CONFIG_LOCKDEP
	unsigned long flags;

	/*
	 * If lockdep gives a backtrace here, please reference
	 * the synchronization rules above.
	 */
	local_irq_save(flags);
	lock_map_acquire(&timer->lockdep_map);
	lock_map_release(&timer->lockdep_map);
	local_irq_restore(flags);
#endif
	/*
	 * don't use it in hardirq context, because it
	 * could lead to deadlock.
	 */
	WARN_ON(in_irq() && !(timer->flags & TIMER_IRQSAFE));
	for (;;) {
		int ret = try_to_del_timer_sync(timer);
		if (ret >= 0)
			return ret;
		cpu_relax();
	}
}