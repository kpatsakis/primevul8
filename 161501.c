static struct pool_workqueue *get_work_pwq(struct work_struct *work)
{
	unsigned long data = atomic_long_read(&work->data);

	if (data & WORK_STRUCT_PWQ)
		return (void *)(data & WORK_STRUCT_WQ_DATA_MASK);
	else
		return NULL;
}