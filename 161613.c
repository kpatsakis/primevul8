static void restore_unbound_workers_cpumask(struct worker_pool *pool, int cpu)
{
	static cpumask_t cpumask;
	struct worker *worker;

	lockdep_assert_held(&pool->attach_mutex);

	/* is @cpu allowed for @pool? */
	if (!cpumask_test_cpu(cpu, pool->attrs->cpumask))
		return;

	cpumask_and(&cpumask, pool->attrs->cpumask, cpu_online_mask);

	/* as we're called from CPU_ONLINE, the following shouldn't fail */
	for_each_pool_worker(worker, pool)
		WARN_ON_ONCE(set_cpus_allowed_ptr(worker->task, &cpumask) < 0);
}