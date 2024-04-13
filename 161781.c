static inline void worker_clr_flags(struct worker *worker, unsigned int flags)
{
	struct worker_pool *pool = worker->pool;
	unsigned int oflags = worker->flags;

	WARN_ON_ONCE(worker->task != current);

	worker->flags &= ~flags;

	/*
	 * If transitioning out of NOT_RUNNING, increment nr_running.  Note
	 * that the nested NOT_RUNNING is not a noop.  NOT_RUNNING is mask
	 * of multiple flags, not a single flag.
	 */
	if ((flags & WORKER_NOT_RUNNING) && (oflags & WORKER_NOT_RUNNING))
		if (!(worker->flags & WORKER_NOT_RUNNING))
			atomic_inc(&pool->nr_running);
}