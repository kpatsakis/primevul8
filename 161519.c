static bool too_many_workers(struct worker_pool *pool)
{
	bool managing = mutex_is_locked(&pool->manager_arb);
	int nr_idle = pool->nr_idle + managing; /* manager is considered idle */
	int nr_busy = pool->nr_workers - nr_idle;

	return nr_idle > 2 && (nr_idle - 2) * MAX_IDLE_WORKERS_RATIO >= nr_busy;
}