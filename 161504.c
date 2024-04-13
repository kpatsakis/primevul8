static void worker_attach_to_pool(struct worker *worker,
				   struct worker_pool *pool)
{
	mutex_lock(&pool->attach_mutex);

	/*
	 * set_cpus_allowed_ptr() will fail if the cpumask doesn't have any
	 * online CPUs.  It'll be re-applied when any of the CPUs come up.
	 */
	set_cpus_allowed_ptr(worker->task, pool->attrs->cpumask);

	/*
	 * The pool->attach_mutex ensures %POOL_DISASSOCIATED remains
	 * stable across this function.  See the comments above the
	 * flag definition for details.
	 */
	if (pool->flags & POOL_DISASSOCIATED)
		worker->flags |= WORKER_UNBOUND;

	list_add_tail(&worker->node, &pool->workers);

	mutex_unlock(&pool->attach_mutex);
}