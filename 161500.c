static void wq_update_unbound_numa(struct workqueue_struct *wq, int cpu,
				   bool online)
{
	int node = cpu_to_node(cpu);
	int cpu_off = online ? -1 : cpu;
	struct pool_workqueue *old_pwq = NULL, *pwq;
	struct workqueue_attrs *target_attrs;
	cpumask_t *cpumask;

	lockdep_assert_held(&wq_pool_mutex);

	if (!wq_numa_enabled || !(wq->flags & WQ_UNBOUND) ||
	    wq->unbound_attrs->no_numa)
		return;

	/*
	 * We don't wanna alloc/free wq_attrs for each wq for each CPU.
	 * Let's use a preallocated one.  The following buf is protected by
	 * CPU hotplug exclusion.
	 */
	target_attrs = wq_update_unbound_numa_attrs_buf;
	cpumask = target_attrs->cpumask;

	copy_workqueue_attrs(target_attrs, wq->unbound_attrs);
	pwq = unbound_pwq_by_node(wq, node);

	/*
	 * Let's determine what needs to be done.  If the target cpumask is
	 * different from the default pwq's, we need to compare it to @pwq's
	 * and create a new one if they don't match.  If the target cpumask
	 * equals the default pwq's, the default pwq should be used.
	 */
	if (wq_calc_node_cpumask(wq->dfl_pwq->pool->attrs, node, cpu_off, cpumask)) {
		if (cpumask_equal(cpumask, pwq->pool->attrs->cpumask))
			return;
	} else {
		goto use_dfl_pwq;
	}

	/* create a new pwq */
	pwq = alloc_unbound_pwq(wq, target_attrs);
	if (!pwq) {
		pr_warn("workqueue: allocation failed while updating NUMA affinity of \"%s\"\n",
			wq->name);
		goto use_dfl_pwq;
	}

	/* Install the new pwq. */
	mutex_lock(&wq->mutex);
	old_pwq = numa_pwq_tbl_install(wq, node, pwq);
	goto out_unlock;

use_dfl_pwq:
	mutex_lock(&wq->mutex);
	spin_lock_irq(&wq->dfl_pwq->pool->lock);
	get_pwq(wq->dfl_pwq);
	spin_unlock_irq(&wq->dfl_pwq->pool->lock);
	old_pwq = numa_pwq_tbl_install(wq, node, wq->dfl_pwq);
out_unlock:
	mutex_unlock(&wq->mutex);
	put_pwq_unlocked(old_pwq);
}