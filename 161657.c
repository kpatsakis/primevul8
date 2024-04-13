static int try_to_grab_pending(struct work_struct *work, bool is_dwork,
			       unsigned long *flags)
{
	struct worker_pool *pool;
	struct pool_workqueue *pwq;

	local_irq_save(*flags);

	/* try to steal the timer if it exists */
	if (is_dwork) {
		struct delayed_work *dwork = to_delayed_work(work);

		/*
		 * dwork->timer is irqsafe.  If del_timer() fails, it's
		 * guaranteed that the timer is not queued anywhere and not
		 * running on the local CPU.
		 */
		if (likely(del_timer(&dwork->timer)))
			return 1;
	}

	/* try to claim PENDING the normal way */
	if (!test_and_set_bit(WORK_STRUCT_PENDING_BIT, work_data_bits(work)))
		return 0;

	/*
	 * The queueing is in progress, or it is already queued. Try to
	 * steal it from ->worklist without clearing WORK_STRUCT_PENDING.
	 */
	pool = get_work_pool(work);
	if (!pool)
		goto fail;

	spin_lock(&pool->lock);
	/*
	 * work->data is guaranteed to point to pwq only while the work
	 * item is queued on pwq->wq, and both updating work->data to point
	 * to pwq on queueing and to pool on dequeueing are done under
	 * pwq->pool->lock.  This in turn guarantees that, if work->data
	 * points to pwq which is associated with a locked pool, the work
	 * item is currently queued on that pool.
	 */
	pwq = get_work_pwq(work);
	if (pwq && pwq->pool == pool) {
		debug_work_deactivate(work);

		/*
		 * A delayed work item cannot be grabbed directly because
		 * it might have linked NO_COLOR work items which, if left
		 * on the delayed_list, will confuse pwq->nr_active
		 * management later on and cause stall.  Make sure the work
		 * item is activated before grabbing.
		 */
		if (*work_data_bits(work) & WORK_STRUCT_DELAYED)
			pwq_activate_delayed_work(work);

		list_del_init(&work->entry);
		pwq_dec_nr_in_flight(pwq, get_work_color(work));

		/* work->data points to pwq iff queued, point to pool */
		set_work_pool_and_keep_pending(work, pool->id);

		spin_unlock(&pool->lock);
		return 1;
	}
	spin_unlock(&pool->lock);
fail:
	local_irq_restore(*flags);
	if (work_is_canceling(work))
		return -ENOENT;
	cpu_relax();
	return -EAGAIN;
}