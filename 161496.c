unsigned int work_busy(struct work_struct *work)
{
	struct worker_pool *pool;
	unsigned long flags;
	unsigned int ret = 0;

	if (work_pending(work))
		ret |= WORK_BUSY_PENDING;

	local_irq_save(flags);
	pool = get_work_pool(work);
	if (pool) {
		spin_lock(&pool->lock);
		if (find_worker_executing_work(pool, work))
			ret |= WORK_BUSY_RUNNING;
		spin_unlock(&pool->lock);
	}
	local_irq_restore(flags);

	return ret;
}