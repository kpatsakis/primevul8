static void apply_wqattrs_lock(void)
{
	/* CPUs should stay stable across pwq creations and installations */
	get_online_cpus();
	mutex_lock(&wq_pool_mutex);
}