static bool need_to_create_worker(struct worker_pool *pool)
{
	return need_more_worker(pool) && !may_start_working(pool);
}