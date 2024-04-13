static void set_work_pool_and_keep_pending(struct work_struct *work,
					   int pool_id)
{
	set_work_data(work, (unsigned long)pool_id << WORK_OFFQ_POOL_SHIFT,
		      WORK_STRUCT_PENDING);
}