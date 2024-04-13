static void pool_mayday_timeout(unsigned long __pool)
{
	struct worker_pool *pool = (void *)__pool;
	struct work_struct *work;

	spin_lock_irq(&pool->lock);
	spin_lock(&wq_mayday_lock);		/* for wq->maydays */

	if (need_to_create_worker(pool)) {
		/*
		 * We've been trying to create a new worker but
		 * haven't been successful.  We might be hitting an
		 * allocation deadlock.  Send distress signals to
		 * rescuers.
		 */
		list_for_each_entry(work, &pool->worklist, entry)
			send_mayday(work);
	}

	spin_unlock(&wq_mayday_lock);
	spin_unlock_irq(&pool->lock);

	mod_timer(&pool->mayday_timer, jiffies + MAYDAY_INTERVAL);
}