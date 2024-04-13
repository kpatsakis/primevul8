static int init_worker_pool(struct worker_pool *pool)
{
	spin_lock_init(&pool->lock);
	pool->id = -1;
	pool->cpu = -1;
	pool->node = NUMA_NO_NODE;
	pool->flags |= POOL_DISASSOCIATED;
	pool->watchdog_ts = jiffies;
	INIT_LIST_HEAD(&pool->worklist);
	INIT_LIST_HEAD(&pool->idle_list);
	hash_init(pool->busy_hash);

	init_timer_deferrable(&pool->idle_timer);
	pool->idle_timer.function = idle_worker_timeout;
	pool->idle_timer.data = (unsigned long)pool;

	setup_timer(&pool->mayday_timer, pool_mayday_timeout,
		    (unsigned long)pool);

	mutex_init(&pool->manager_arb);
	mutex_init(&pool->attach_mutex);
	INIT_LIST_HEAD(&pool->workers);

	ida_init(&pool->worker_ida);
	INIT_HLIST_NODE(&pool->hash_node);
	pool->refcnt = 1;

	/* shouldn't fail above this point */
	pool->attrs = alloc_workqueue_attrs(GFP_KERNEL);
	if (!pool->attrs)
		return -ENOMEM;
	return 0;
}