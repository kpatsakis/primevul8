int workqueue_set_unbound_cpumask(cpumask_var_t cpumask)
{
	int ret = -EINVAL;
	cpumask_var_t saved_cpumask;

	if (!zalloc_cpumask_var(&saved_cpumask, GFP_KERNEL))
		return -ENOMEM;

	cpumask_and(cpumask, cpumask, cpu_possible_mask);
	if (!cpumask_empty(cpumask)) {
		apply_wqattrs_lock();

		/* save the old wq_unbound_cpumask. */
		cpumask_copy(saved_cpumask, wq_unbound_cpumask);

		/* update wq_unbound_cpumask at first and apply it to wqs. */
		cpumask_copy(wq_unbound_cpumask, cpumask);
		ret = workqueue_apply_unbound_cpumask();

		/* restore the wq_unbound_cpumask when failed. */
		if (ret < 0)
			cpumask_copy(wq_unbound_cpumask, saved_cpumask);

		apply_wqattrs_unlock();
	}

	free_cpumask_var(saved_cpumask);
	return ret;
}