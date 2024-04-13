int workqueue_online_cpu(unsigned int cpu)
{
	struct worker_pool *pool;
	struct workqueue_struct *wq;
	int pi;

	mutex_lock(&wq_pool_mutex);

	for_each_pool(pool, pi) {
		mutex_lock(&pool->attach_mutex);

		if (pool->cpu == cpu)
			rebind_workers(pool);
		else if (pool->cpu < 0)
			restore_unbound_workers_cpumask(pool, cpu);

		mutex_unlock(&pool->attach_mutex);
	}

	/* update NUMA affinity of unbound workqueues */
	list_for_each_entry(wq, &workqueues, list)
		wq_update_unbound_numa(wq, cpu, true);

	mutex_unlock(&wq_pool_mutex);
	return 0;
}