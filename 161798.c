static void __init wq_numa_init(void)
{
	cpumask_var_t *tbl;
	int node, cpu;

	if (num_possible_nodes() <= 1)
		return;

	if (wq_disable_numa) {
		pr_info("workqueue: NUMA affinity support disabled\n");
		return;
	}

	wq_update_unbound_numa_attrs_buf = alloc_workqueue_attrs(GFP_KERNEL);
	BUG_ON(!wq_update_unbound_numa_attrs_buf);

	/*
	 * We want masks of possible CPUs of each node which isn't readily
	 * available.  Build one from cpu_to_node() which should have been
	 * fully initialized by now.
	 */
	tbl = kzalloc(nr_node_ids * sizeof(tbl[0]), GFP_KERNEL);
	BUG_ON(!tbl);

	for_each_node(node)
		BUG_ON(!zalloc_cpumask_var_node(&tbl[node], GFP_KERNEL,
				node_online(node) ? node : NUMA_NO_NODE));

	for_each_possible_cpu(cpu) {
		node = cpu_to_node(cpu);
		if (WARN_ON(node == NUMA_NO_NODE)) {
			pr_warn("workqueue: NUMA node mapping not available for cpu%d, disabling NUMA support\n", cpu);
			/* happens iff arch is bonkers, let's just proceed */
			return;
		}
		cpumask_set_cpu(cpu, tbl[node]);
	}

	wq_numa_possible_cpumask = tbl;
	wq_numa_enabled = true;
}