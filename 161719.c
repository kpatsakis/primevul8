static void link_pwq(struct pool_workqueue *pwq)
{
	struct workqueue_struct *wq = pwq->wq;

	lockdep_assert_held(&wq->mutex);

	/* may be called multiple times, ignore if already linked */
	if (!list_empty(&pwq->pwqs_node))
		return;

	/* set the matching work_color */
	pwq->work_color = wq->work_color;

	/* sync max_active to the current setting */
	pwq_adjust_max_active(pwq);

	/* link in @pwq */
	list_add_rcu(&pwq->pwqs_node, &wq->pwqs);
}