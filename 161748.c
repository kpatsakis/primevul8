void show_workqueue_state(void)
{
	struct workqueue_struct *wq;
	struct worker_pool *pool;
	unsigned long flags;
	int pi;

	rcu_read_lock_sched();

	pr_info("Showing busy workqueues and worker pools:\n");

	list_for_each_entry_rcu(wq, &workqueues, list) {
		struct pool_workqueue *pwq;
		bool idle = true;

		for_each_pwq(pwq, wq) {
			if (pwq->nr_active || !list_empty(&pwq->delayed_works)) {
				idle = false;
				break;
			}
		}
		if (idle)
			continue;

		pr_info("workqueue %s: flags=0x%x\n", wq->name, wq->flags);

		for_each_pwq(pwq, wq) {
			spin_lock_irqsave(&pwq->pool->lock, flags);
			if (pwq->nr_active || !list_empty(&pwq->delayed_works))
				show_pwq(pwq);
			spin_unlock_irqrestore(&pwq->pool->lock, flags);
		}
	}

	for_each_pool(pool, pi) {
		struct worker *worker;
		bool first = true;

		spin_lock_irqsave(&pool->lock, flags);
		if (pool->nr_workers == pool->nr_idle)
			goto next_pool;

		pr_info("pool %d:", pool->id);
		pr_cont_pool_info(pool);
		pr_cont(" hung=%us workers=%d",
			jiffies_to_msecs(jiffies - pool->watchdog_ts) / 1000,
			pool->nr_workers);
		if (pool->manager)
			pr_cont(" manager: %d",
				task_pid_nr(pool->manager->task));
		list_for_each_entry(worker, &pool->idle_list, entry) {
			pr_cont(" %s%d", first ? "idle: " : "",
				task_pid_nr(worker->task));
			first = false;
		}
		pr_cont("\n");
	next_pool:
		spin_unlock_irqrestore(&pool->lock, flags);
	}

	rcu_read_unlock_sched();
}