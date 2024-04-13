ttwu_stat(struct task_struct *p, int cpu, int wake_flags)
{
#ifdef CONFIG_SCHEDSTATS
	struct rq *rq = this_rq();

#ifdef CONFIG_SMP
	int this_cpu = smp_processor_id();

	if (cpu == this_cpu) {
		schedstat_inc(rq, ttwu_local);
		schedstat_inc(p, se.statistics.nr_wakeups_local);
	} else {
		struct sched_domain *sd;

		schedstat_inc(p, se.statistics.nr_wakeups_remote);
		rcu_read_lock();
		for_each_domain(this_cpu, sd) {
			if (cpumask_test_cpu(cpu, sched_domain_span(sd))) {
				schedstat_inc(sd, ttwu_wake_remote);
				break;
			}
		}
		rcu_read_unlock();
	}

	if (wake_flags & WF_MIGRATED)
		schedstat_inc(p, se.statistics.nr_wakeups_migrate);

#endif /* CONFIG_SMP */

	schedstat_inc(rq, ttwu_count);
	schedstat_inc(p, se.statistics.nr_wakeups);

	if (wake_flags & WF_SYNC)
		schedstat_inc(p, se.statistics.nr_wakeups_sync);

#endif /* CONFIG_SCHEDSTATS */
}