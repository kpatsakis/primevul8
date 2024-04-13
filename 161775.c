void thaw_workqueues(void)
{
	struct workqueue_struct *wq;
	struct pool_workqueue *pwq;

	mutex_lock(&wq_pool_mutex);

	if (!workqueue_freezing)
		goto out_unlock;

	workqueue_freezing = false;

	/* restore max_active and repopulate worklist */
	list_for_each_entry(wq, &workqueues, list) {
		mutex_lock(&wq->mutex);
		for_each_pwq(pwq, wq)
			pwq_adjust_max_active(pwq);
		mutex_unlock(&wq->mutex);
	}

out_unlock:
	mutex_unlock(&wq_pool_mutex);
}