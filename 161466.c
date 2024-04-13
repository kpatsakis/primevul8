static void worker_detach_from_pool(struct worker *worker,
				    struct worker_pool *pool)
{
	struct completion *detach_completion = NULL;

	mutex_lock(&pool->attach_mutex);
	list_del(&worker->node);
	if (list_empty(&pool->workers))
		detach_completion = pool->detach_completion;
	mutex_unlock(&pool->attach_mutex);

	/* clear leftover flags without pool->lock after it is detached */
	worker->flags &= ~(WORKER_UNBOUND | WORKER_REBOUND);

	if (detach_completion)
		complete(detach_completion);
}