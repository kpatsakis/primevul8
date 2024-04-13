static void put_pwq_unlocked(struct pool_workqueue *pwq)
{
	if (pwq) {
		/*
		 * As both pwqs and pools are sched-RCU protected, the
		 * following lock operations are safe.
		 */
		spin_lock_irq(&pwq->pool->lock);
		put_pwq(pwq);
		spin_unlock_irq(&pwq->pool->lock);
	}
}