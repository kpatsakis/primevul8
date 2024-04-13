static void wq_unbind_fn(struct work_struct *work)
{
	int cpu = smp_processor_id();
	struct worker_pool *pool;
	struct worker *worker;

	for_each_cpu_worker_pool(pool, cpu) {
		mutex_lock(&pool->attach_mutex);
		spin_lock_irq(&pool->lock);

		/*
		 * We've blocked all attach/detach operations. Make all workers
		 * unbound and set DISASSOCIATED.  Before this, all workers
		 * except for the ones which are still executing works from
		 * before the last CPU down must be on the cpu.  After
		 * this, they may become diasporas.
		 */
		for_each_pool_worker(worker, pool)
			worker->flags |= WORKER_UNBOUND;

		pool->flags |= POOL_DISASSOCIATED;

		spin_unlock_irq(&pool->lock);
		mutex_unlock(&pool->attach_mutex);

		/*
		 * Call schedule() so that we cross rq->lock and thus can
		 * guarantee sched callbacks see the %WORKER_UNBOUND flag.
		 * This is necessary as scheduler callbacks may be invoked
		 * from other cpus.
		 */
		schedule();

		/*
		 * Sched callbacks are disabled now.  Zap nr_running.
		 * After this, nr_running stays zero and need_more_worker()
		 * and keep_working() are always true as long as the
		 * worklist is not empty.  This pool now behaves as an
		 * unbound (in terms of concurrency management) pool which
		 * are served by workers tied to the pool.
		 */
		atomic_set(&pool->nr_running, 0);

		/*
		 * With concurrency management just turned off, a busy
		 * worker blocking could lead to lengthy stalls.  Kick off
		 * unbound chain execution of currently pending work items.
		 */
		spin_lock_irq(&pool->lock);
		wake_up_worker(pool);
		spin_unlock_irq(&pool->lock);
	}
}