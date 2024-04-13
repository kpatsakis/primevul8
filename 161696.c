static void destroy_worker(struct worker *worker)
{
	struct worker_pool *pool = worker->pool;

	lockdep_assert_held(&pool->lock);

	/* sanity check frenzy */
	if (WARN_ON(worker->current_work) ||
	    WARN_ON(!list_empty(&worker->scheduled)) ||
	    WARN_ON(!(worker->flags & WORKER_IDLE)))
		return;

	pool->nr_workers--;
	pool->nr_idle--;

	list_del_init(&worker->entry);
	worker->flags |= WORKER_DIE;
	wake_up_process(worker->task);
}