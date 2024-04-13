void destroy_workqueue(struct workqueue_struct *wq)
{
	struct pool_workqueue *pwq;
	int node;

	/* drain it before proceeding with destruction */
	drain_workqueue(wq);

	/* sanity checks */
	mutex_lock(&wq->mutex);
	for_each_pwq(pwq, wq) {
		int i;

		for (i = 0; i < WORK_NR_COLORS; i++) {
			if (WARN_ON(pwq->nr_in_flight[i])) {
				mutex_unlock(&wq->mutex);
				show_workqueue_state();
				return;
			}
		}

		if (WARN_ON((pwq != wq->dfl_pwq) && (pwq->refcnt > 1)) ||
		    WARN_ON(pwq->nr_active) ||
		    WARN_ON(!list_empty(&pwq->delayed_works))) {
			mutex_unlock(&wq->mutex);
			show_workqueue_state();
			return;
		}
	}
	mutex_unlock(&wq->mutex);

	/*
	 * wq list is used to freeze wq, remove from list after
	 * flushing is complete in case freeze races us.
	 */
	mutex_lock(&wq_pool_mutex);
	list_del_rcu(&wq->list);
	mutex_unlock(&wq_pool_mutex);

	workqueue_sysfs_unregister(wq);

	if (wq->rescuer)
		kthread_stop(wq->rescuer->task);

	if (!(wq->flags & WQ_UNBOUND)) {
		/*
		 * The base ref is never dropped on per-cpu pwqs.  Directly
		 * schedule RCU free.
		 */
		call_rcu_sched(&wq->rcu, rcu_free_wq);
	} else {
		/*
		 * We're the sole accessor of @wq at this point.  Directly
		 * access numa_pwq_tbl[] and dfl_pwq to put the base refs.
		 * @wq will be freed when the last pwq is released.
		 */
		for_each_node(node) {
			pwq = rcu_access_pointer(wq->numa_pwq_tbl[node]);
			RCU_INIT_POINTER(wq->numa_pwq_tbl[node], NULL);
			put_pwq_unlocked(pwq);
		}

		/*
		 * Put dfl_pwq.  @wq may be freed any time after dfl_pwq is
		 * put.  Don't access it afterwards.
		 */
		pwq = wq->dfl_pwq;
		wq->dfl_pwq = NULL;
		put_pwq_unlocked(pwq);
	}
}