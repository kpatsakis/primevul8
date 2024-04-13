static bool need_more_worker(struct worker_pool *pool)
{
	return !list_empty(&pool->worklist) && __need_more_worker(pool);
}