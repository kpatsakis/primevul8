static bool keep_working(struct worker_pool *pool)
{
	return !list_empty(&pool->worklist) &&
		atomic_read(&pool->nr_running) <= 1;
}