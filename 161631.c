void drain_workqueue(struct workqueue_struct *wq)
{
	unsigned int flush_cnt = 0;
	struct pool_workqueue *pwq;

	/*
	 * __queue_work() needs to test whether there are drainers, is much
	 * hotter than drain_workqueue() and already looks at @wq->flags.
	 * Use __WQ_DRAINING so that queue doesn't have to check nr_drainers.
	 */
	mutex_lock(&wq->mutex);
	if (!wq->nr_drainers++)
		wq->flags |= __WQ_DRAINING;
	mutex_unlock(&wq->mutex);
reflush:
	flush_workqueue(wq);

	mutex_lock(&wq->mutex);

	for_each_pwq(pwq, wq) {
		bool drained;

		spin_lock_irq(&pwq->pool->lock);
		drained = !pwq->nr_active && list_empty(&pwq->delayed_works);
		spin_unlock_irq(&pwq->pool->lock);

		if (drained)
			continue;

		if (++flush_cnt == 10 ||
		    (flush_cnt % 100 == 0 && flush_cnt <= 1000))
			pr_warn("workqueue %s: drain_workqueue() isn't complete after %u tries\n",
				wq->name, flush_cnt);

		mutex_unlock(&wq->mutex);
		goto reflush;
	}

	if (!--wq->nr_drainers)
		wq->flags &= ~__WQ_DRAINING;
	mutex_unlock(&wq->mutex);
}