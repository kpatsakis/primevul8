static bool start_flush_work(struct work_struct *work, struct wq_barrier *barr)
{
	struct worker *worker = NULL;
	struct worker_pool *pool;
	struct pool_workqueue *pwq;

	might_sleep();

	local_irq_disable();
	pool = get_work_pool(work);
	if (!pool) {
		local_irq_enable();
		return false;
	}

	spin_lock(&pool->lock);
	/* see the comment in try_to_grab_pending() with the same code */
	pwq = get_work_pwq(work);
	if (pwq) {
		if (unlikely(pwq->pool != pool))
			goto already_gone;
	} else {
		worker = find_worker_executing_work(pool, work);
		if (!worker)
			goto already_gone;
		pwq = worker->current_pwq;
	}

	check_flush_dependency(pwq->wq, work);

	insert_wq_barrier(pwq, barr, work, worker);
	spin_unlock_irq(&pool->lock);

	/*
	 * If @max_active is 1 or rescuer is in use, flushing another work
	 * item on the same workqueue may lead to deadlock.  Make sure the
	 * flusher is not running on the same workqueue by verifying write
	 * access.
	 */
	if (pwq->wq->saved_max_active == 1 || pwq->wq->rescuer)
		lock_map_acquire(&pwq->wq->lockdep_map);
	else
		lock_map_acquire_read(&pwq->wq->lockdep_map);
	lock_map_release(&pwq->wq->lockdep_map);

	return true;
already_gone:
	spin_unlock_irq(&pool->lock);
	return false;
}