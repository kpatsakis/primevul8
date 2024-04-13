static void pwq_adjust_max_active(struct pool_workqueue *pwq)
{
	struct workqueue_struct *wq = pwq->wq;
	bool freezable = wq->flags & WQ_FREEZABLE;
	unsigned long flags;

	/* for @wq->saved_max_active */
	lockdep_assert_held(&wq->mutex);

	/* fast exit for non-freezable wqs */
	if (!freezable && pwq->max_active == wq->saved_max_active)
		return;

	/* this function can be called during early boot w/ irq disabled */
	spin_lock_irqsave(&pwq->pool->lock, flags);

	/*
	 * During [un]freezing, the caller is responsible for ensuring that
	 * this function is called at least once after @workqueue_freezing
	 * is updated and visible.
	 */
	if (!freezable || !workqueue_freezing) {
		pwq->max_active = wq->saved_max_active;

		while (!list_empty(&pwq->delayed_works) &&
		       pwq->nr_active < pwq->max_active)
			pwq_activate_first_delayed(pwq);

		/*
		 * Need to kick a worker after thawed or an unbound wq's
		 * max_active is bumped.  It's a slow path.  Do it always.
		 */
		wake_up_worker(pwq->pool);
	} else {
		pwq->max_active = 0;
	}

	spin_unlock_irqrestore(&pwq->pool->lock, flags);
}