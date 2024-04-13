bool workqueue_congested(int cpu, struct workqueue_struct *wq)
{
	struct pool_workqueue *pwq;
	bool ret;

	rcu_read_lock_sched();

	if (cpu == WORK_CPU_UNBOUND)
		cpu = smp_processor_id();

	if (!(wq->flags & WQ_UNBOUND))
		pwq = per_cpu_ptr(wq->cpu_pwqs, cpu);
	else
		pwq = unbound_pwq_by_node(wq, cpu_to_node(cpu));

	ret = !list_empty(&pwq->delayed_works);
	rcu_read_unlock_sched();

	return ret;
}