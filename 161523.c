static int get_work_pool_id(struct work_struct *work)
{
	unsigned long data = atomic_long_read(&work->data);

	if (data & WORK_STRUCT_PWQ)
		return ((struct pool_workqueue *)
			(data & WORK_STRUCT_WQ_DATA_MASK))->pool->id;

	return data >> WORK_OFFQ_POOL_SHIFT;
}