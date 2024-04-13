static struct worker_pool *get_work_pool(struct work_struct *work)
{
	unsigned long data = atomic_long_read(&work->data);
	int pool_id;

	assert_rcu_or_pool_mutex();

	if (data & WORK_STRUCT_PWQ)
		return ((struct pool_workqueue *)
			(data & WORK_STRUCT_WQ_DATA_MASK))->pool;

	pool_id = data >> WORK_OFFQ_POOL_SHIFT;
	if (pool_id == WORK_OFFQ_POOL_NONE)
		return NULL;

	return idr_find(&worker_pool_idr, pool_id);
}