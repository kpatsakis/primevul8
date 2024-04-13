static void put_pwq(struct pool_workqueue *pwq)
{
	lockdep_assert_held(&pwq->pool->lock);
	if (likely(--pwq->refcnt))
		return;
	if (WARN_ON_ONCE(!(pwq->wq->flags & WQ_UNBOUND)))
		return;
	/*
	 * @pwq can't be released under pool->lock, bounce to
	 * pwq_unbound_release_workfn().  This never recurses on the same
	 * pool->lock as this path is taken only for unbound workqueues and
	 * the release work item is scheduled on a per-cpu workqueue.  To
	 * avoid lockdep warning, unbound pool->locks are given lockdep
	 * subclass of 1 in get_unbound_pool().
	 */
	schedule_work(&pwq->unbound_release_work);
}