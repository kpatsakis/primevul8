static void mark_work_canceling(struct work_struct *work)
{
	unsigned long pool_id = get_work_pool_id(work);

	pool_id <<= WORK_OFFQ_POOL_SHIFT;
	set_work_data(work, pool_id | WORK_OFFQ_CANCELING, WORK_STRUCT_PENDING);
}