int workqueue_offline_cpu(unsigned int cpu)
{
	struct work_struct unbind_work;
	struct workqueue_struct *wq;

	/* unbinding per-cpu workers should happen on the local CPU */
	INIT_WORK_ONSTACK(&unbind_work, wq_unbind_fn);
	queue_work_on(cpu, system_highpri_wq, &unbind_work);

	/* update NUMA affinity of unbound workqueues */
	mutex_lock(&wq_pool_mutex);
	list_for_each_entry(wq, &workqueues, list)
		wq_update_unbound_numa(wq, cpu, false);
	mutex_unlock(&wq_pool_mutex);

	/* wait for per-cpu unbinding to finish */
	flush_work(&unbind_work);
	destroy_work_on_stack(&unbind_work);
	return 0;
}