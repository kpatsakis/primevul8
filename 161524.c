static int wq_select_unbound_cpu(int cpu)
{
	static bool printed_dbg_warning;
	int new_cpu;

	if (likely(!wq_debug_force_rr_cpu)) {
		if (cpumask_test_cpu(cpu, wq_unbound_cpumask))
			return cpu;
	} else if (!printed_dbg_warning) {
		pr_warn("workqueue: round-robin CPU selection forced, expect performance impact\n");
		printed_dbg_warning = true;
	}

	if (cpumask_empty(wq_unbound_cpumask))
		return cpu;

	new_cpu = __this_cpu_read(wq_rr_cpu_last);
	new_cpu = cpumask_next_and(new_cpu, wq_unbound_cpumask, cpu_online_mask);
	if (unlikely(new_cpu >= nr_cpu_ids)) {
		new_cpu = cpumask_first_and(wq_unbound_cpumask, cpu_online_mask);
		if (unlikely(new_cpu >= nr_cpu_ids))
			return cpu;
	}
	__this_cpu_write(wq_rr_cpu_last, new_cpu);

	return new_cpu;
}