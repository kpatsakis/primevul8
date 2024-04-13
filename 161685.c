static void send_mayday(struct work_struct *work)
{
	struct pool_workqueue *pwq = get_work_pwq(work);
	struct workqueue_struct *wq = pwq->wq;

	lockdep_assert_held(&wq_mayday_lock);

	if (!wq->rescuer)
		return;

	/* mayday mayday mayday */
	if (list_empty(&pwq->mayday_node)) {
		/*
		 * If @pwq is for an unbound wq, its base ref may be put at
		 * any time due to an attribute change.  Pin @pwq until the
		 * rescuer is done with it.
		 */
		get_pwq(pwq);
		list_add_tail(&pwq->mayday_node, &wq->maydays);
		wake_up_process(wq->rescuer->task);
	}
}