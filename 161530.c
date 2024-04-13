static void put_unbound_pool(struct worker_pool *pool)
{
	DECLARE_COMPLETION_ONSTACK(detach_completion);
	struct worker *worker;

	lockdep_assert_held(&wq_pool_mutex);

	if (--pool->refcnt)
		return;

	/* sanity checks */
	if (WARN_ON(!(pool->cpu < 0)) ||
	    WARN_ON(!list_empty(&pool->worklist)))
		return;

	/* release id and unhash */
	if (pool->id >= 0)
		idr_remove(&worker_pool_idr, pool->id);
	hash_del(&pool->hash_node);

	/*
	 * Become the manager and destroy all workers.  Grabbing
	 * manager_arb prevents @pool's workers from blocking on
	 * attach_mutex.
	 */
	mutex_lock(&pool->manager_arb);

	spin_lock_irq(&pool->lock);
	while ((worker = first_idle_worker(pool)))
		destroy_worker(worker);
	WARN_ON(pool->nr_workers || pool->nr_idle);
	spin_unlock_irq(&pool->lock);

	mutex_lock(&pool->attach_mutex);
	if (!list_empty(&pool->workers))
		pool->detach_completion = &detach_completion;
	mutex_unlock(&pool->attach_mutex);

	if (pool->detach_completion)
		wait_for_completion(pool->detach_completion);

	mutex_unlock(&pool->manager_arb);

	/* shut down the timers */
	del_timer_sync(&pool->idle_timer);
	del_timer_sync(&pool->mayday_timer);

	/* sched-RCU protected to allow dereferences from get_work_pool() */
	call_rcu_sched(&pool->rcu, rcu_free_pool);
}