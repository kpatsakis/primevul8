static bool may_start_working(struct worker_pool *pool)
{
	return pool->nr_idle;
}