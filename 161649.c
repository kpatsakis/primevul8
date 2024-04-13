static void pwq_activate_delayed_work(struct work_struct *work)
{
	struct pool_workqueue *pwq = get_work_pwq(work);

	trace_workqueue_activate_work(work);
	if (list_empty(&pwq->pool->worklist))
		pwq->pool->watchdog_ts = jiffies;
	move_linked_works(work, &pwq->pool->worklist, NULL);
	__clear_bit(WORK_STRUCT_DELAYED_BIT, work_data_bits(work));
	pwq->nr_active++;
}