static int worker_pool_assign_id(struct worker_pool *pool)
{
	int ret;

	lockdep_assert_held(&wq_pool_mutex);

	ret = idr_alloc(&worker_pool_idr, pool, 0, WORK_OFFQ_POOL_NONE,
			GFP_KERNEL);
	if (ret >= 0) {
		pool->id = ret;
		return 0;
	}
	return ret;
}