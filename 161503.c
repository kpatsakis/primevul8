static int rescuer_thread(void *__rescuer)
{
	struct worker *rescuer = __rescuer;
	struct workqueue_struct *wq = rescuer->rescue_wq;
	struct list_head *scheduled = &rescuer->scheduled;
	bool should_stop;

	set_user_nice(current, RESCUER_NICE_LEVEL);

	/*
	 * Mark rescuer as worker too.  As WORKER_PREP is never cleared, it
	 * doesn't participate in concurrency management.
	 */
	rescuer->task->flags |= PF_WQ_WORKER;
repeat:
	set_current_state(TASK_INTERRUPTIBLE);

	/*
	 * By the time the rescuer is requested to stop, the workqueue
	 * shouldn't have any work pending, but @wq->maydays may still have
	 * pwq(s) queued.  This can happen by non-rescuer workers consuming
	 * all the work items before the rescuer got to them.  Go through
	 * @wq->maydays processing before acting on should_stop so that the
	 * list is always empty on exit.
	 */
	should_stop = kthread_should_stop();

	/* see whether any pwq is asking for help */
	spin_lock_irq(&wq_mayday_lock);

	while (!list_empty(&wq->maydays)) {
		struct pool_workqueue *pwq = list_first_entry(&wq->maydays,
					struct pool_workqueue, mayday_node);
		struct worker_pool *pool = pwq->pool;
		struct work_struct *work, *n;
		bool first = true;

		__set_current_state(TASK_RUNNING);
		list_del_init(&pwq->mayday_node);

		spin_unlock_irq(&wq_mayday_lock);

		worker_attach_to_pool(rescuer, pool);

		spin_lock_irq(&pool->lock);
		rescuer->pool = pool;

		/*
		 * Slurp in all works issued via this workqueue and
		 * process'em.
		 */
		WARN_ON_ONCE(!list_empty(scheduled));
		list_for_each_entry_safe(work, n, &pool->worklist, entry) {
			if (get_work_pwq(work) == pwq) {
				if (first)
					pool->watchdog_ts = jiffies;
				move_linked_works(work, scheduled, &n);
			}
			first = false;
		}

		if (!list_empty(scheduled)) {
			process_scheduled_works(rescuer);

			/*
			 * The above execution of rescued work items could
			 * have created more to rescue through
			 * pwq_activate_first_delayed() or chained
			 * queueing.  Let's put @pwq back on mayday list so
			 * that such back-to-back work items, which may be
			 * being used to relieve memory pressure, don't
			 * incur MAYDAY_INTERVAL delay inbetween.
			 */
			if (need_to_create_worker(pool)) {
				spin_lock(&wq_mayday_lock);
				get_pwq(pwq);
				list_move_tail(&pwq->mayday_node, &wq->maydays);
				spin_unlock(&wq_mayday_lock);
			}
		}

		/*
		 * Put the reference grabbed by send_mayday().  @pool won't
		 * go away while we're still attached to it.
		 */
		put_pwq(pwq);

		/*
		 * Leave this pool.  If need_more_worker() is %true, notify a
		 * regular worker; otherwise, we end up with 0 concurrency
		 * and stalling the execution.
		 */
		if (need_more_worker(pool))
			wake_up_worker(pool);

		rescuer->pool = NULL;
		spin_unlock_irq(&pool->lock);

		worker_detach_from_pool(rescuer, pool);

		spin_lock_irq(&wq_mayday_lock);
	}

	spin_unlock_irq(&wq_mayday_lock);

	if (should_stop) {
		__set_current_state(TASK_RUNNING);
		rescuer->task->flags &= ~PF_WQ_WORKER;
		return 0;
	}

	/* rescuers should never participate in concurrency management */
	WARN_ON_ONCE(!(rescuer->flags & WORKER_NOT_RUNNING));
	schedule();
	goto repeat;
}