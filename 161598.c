int timer_migration_handler(struct ctl_table *table, int write,
			    void __user *buffer, size_t *lenp,
			    loff_t *ppos)
{
	static DEFINE_MUTEX(mutex);
	int ret;

	mutex_lock(&mutex);
	ret = proc_dointvec(table, write, buffer, lenp, ppos);
	if (!ret && write)
		timers_update_migration(false);
	mutex_unlock(&mutex);
	return ret;
}