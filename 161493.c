static void rebind_workers(struct worker_pool *pool)
{
	struct worker *worker;

	lockdep_assert_held(&pool->attach_mutex);

	/*
	 * Restore CPU affinity of all workers.  As all idle workers should
	 * be on the run-queue of the associated CPU before any local
	 * wake-ups for concurrency management happen, restore CPU affinity
	 * of all workers first and then clear UNBOUND.  As we're called
	 * from CPU_ONLINE, the following shouldn't fail.
	 */
	for_each_pool_worker(worker, pool)
		WARN_ON_ONCE(set_cpus_allowed_ptr(worker->task,
						  pool->attrs->cpumask) < 0);

	spin_lock_irq(&pool->lock);

	/*
	 * XXX: CPU hotplug notifiers are weird and can call DOWN_FAILED
	 * w/o preceding DOWN_PREPARE.  Work around it.  CPU hotplug is
	 * being reworked and this can go away in time.
	 */
	if (!(pool->flags & POOL_DISASSOCIATED)) {
		spin_unlock_irq(&pool->lock);
		return;
	}

	pool->flags &= ~POOL_DISASSOCIATED;

	for_each_pool_worker(worker, pool) {
		unsigned int worker_flags = worker->flags;

		/*
		 * A bound idle worker should actually be on the runqueue
		 * of the associated CPU for local wake-ups targeting it to
		 * work.  Kick all idle workers so that they migrate to the
		 * associated CPU.  Doing this in the same loop as
		 * replacing UNBOUND with REBOUND is safe as no worker will
		 * be bound before @pool->lock is released.
		 */
		if (worker_flags & WORKER_IDLE)
			wake_up_process(worker->task);

		/*
		 * We want to clear UNBOUND but can't directly call
		 * worker_clr_flags() or adjust nr_running.  Atomically
		 * replace UNBOUND with another NOT_RUNNING flag REBOUND.
		 * @worker will clear REBOUND using worker_clr_flags() when
		 * it initiates the next execution cycle thus restoring
		 * concurrency management.  Note that when or whether
		 * @worker clears REBOUND doesn't affect correctness.
		 *
		 * ACCESS_ONCE() is necessary because @worker->flags may be
		 * tested without holding any lock in
		 * wq_worker_waking_up().  Without it, NOT_RUNNING test may
		 * fail incorrectly leading to premature concurrency
		 * management operations.
		 */
		WARN_ON_ONCE(!(worker_flags & WORKER_UNBOUND));
		worker_flags |= WORKER_REBOUND;
		worker_flags &= ~WORKER_UNBOUND;
		ACCESS_ONCE(worker->flags) = worker_flags;
	}

	spin_unlock_irq(&pool->lock);
}