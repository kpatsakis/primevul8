static bool manage_workers(struct worker *worker)
{
	struct worker_pool *pool = worker->pool;

	/*
	 * Anyone who successfully grabs manager_arb wins the arbitration
	 * and becomes the manager.  mutex_trylock() on pool->manager_arb
	 * failure while holding pool->lock reliably indicates that someone
	 * else is managing the pool and the worker which failed trylock
	 * can proceed to executing work items.  This means that anyone
	 * grabbing manager_arb is responsible for actually performing
	 * manager duties.  If manager_arb is grabbed and released without
	 * actual management, the pool may stall indefinitely.
	 */
	if (!mutex_trylock(&pool->manager_arb))
		return false;
	pool->manager = worker;

	maybe_create_worker(pool);

	pool->manager = NULL;
	mutex_unlock(&pool->manager_arb);
	return true;
}