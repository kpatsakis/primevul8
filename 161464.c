static void apply_wqattrs_unlock(void)
{
	mutex_unlock(&wq_pool_mutex);
	put_online_cpus();
}