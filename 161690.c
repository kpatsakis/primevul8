static void idle_worker_timeout(unsigned long __pool)
{
	struct worker_pool *pool = (void *)__pool;

	spin_lock_irq(&pool->lock);

	while (too_many_workers(pool)) {
		struct worker *worker;
		unsigned long expires;

		/* idle_list is kept in LIFO order, check the last one */
		worker = list_entry(pool->idle_list.prev, struct worker, entry);
		expires = worker->last_active + IDLE_WORKER_TIMEOUT;

		if (time_before(jiffies, expires)) {
			mod_timer(&pool->idle_timer, expires);
			break;
		}

		destroy_worker(worker);
	}

	spin_unlock_irq(&pool->lock);
}