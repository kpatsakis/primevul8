void workqueue_set_max_active(struct workqueue_struct *wq, int max_active)
{
	struct pool_workqueue *pwq;

	/* disallow meddling with max_active for ordered workqueues */
	if (WARN_ON(wq->flags & __WQ_ORDERED))
		return;

	max_active = wq_clamp_max_active(max_active, wq->flags, wq->name);

	mutex_lock(&wq->mutex);

	wq->saved_max_active = max_active;

	for_each_pwq(pwq, wq)
		pwq_adjust_max_active(pwq);

	mutex_unlock(&wq->mutex);
}