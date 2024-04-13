static void worker_leave_idle(struct worker *worker)
{
	struct worker_pool *pool = worker->pool;

	if (WARN_ON_ONCE(!(worker->flags & WORKER_IDLE)))
		return;
	worker_clr_flags(worker, WORKER_IDLE);
	pool->nr_idle--;
	list_del_init(&worker->entry);
}