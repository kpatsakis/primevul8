static void wq_watchdog_timer_fn(unsigned long data)
{
	unsigned long thresh = READ_ONCE(wq_watchdog_thresh) * HZ;
	bool lockup_detected = false;
	struct worker_pool *pool;
	int pi;

	if (!thresh)
		return;

	rcu_read_lock();

	for_each_pool(pool, pi) {
		unsigned long pool_ts, touched, ts;

		if (list_empty(&pool->worklist))
			continue;

		/* get the latest of pool and touched timestamps */
		pool_ts = READ_ONCE(pool->watchdog_ts);
		touched = READ_ONCE(wq_watchdog_touched);

		if (time_after(pool_ts, touched))
			ts = pool_ts;
		else
			ts = touched;

		if (pool->cpu >= 0) {
			unsigned long cpu_touched =
				READ_ONCE(per_cpu(wq_watchdog_touched_cpu,
						  pool->cpu));
			if (time_after(cpu_touched, ts))
				ts = cpu_touched;
		}

		/* did we stall? */
		if (time_after(jiffies, ts + thresh)) {
			lockup_detected = true;
			pr_emerg("BUG: workqueue lockup - pool");
			pr_cont_pool_info(pool);
			pr_cont(" stuck for %us!\n",
				jiffies_to_msecs(jiffies - pool_ts) / 1000);
		}
	}

	rcu_read_unlock();

	if (lockup_detected)
		show_workqueue_state();

	wq_watchdog_reset_touched();
	mod_timer(&wq_watchdog_timer, jiffies + thresh);
}