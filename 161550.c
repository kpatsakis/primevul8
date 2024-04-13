static void pwq_dec_nr_in_flight(struct pool_workqueue *pwq, int color)
{
	/* uncolored work items don't participate in flushing or nr_active */
	if (color == WORK_NO_COLOR)
		goto out_put;

	pwq->nr_in_flight[color]--;

	pwq->nr_active--;
	if (!list_empty(&pwq->delayed_works)) {
		/* one down, submit a delayed one */
		if (pwq->nr_active < pwq->max_active)
			pwq_activate_first_delayed(pwq);
	}

	/* is flush in progress and are we at the flushing tip? */
	if (likely(pwq->flush_color != color))
		goto out_put;

	/* are there still in-flight works? */
	if (pwq->nr_in_flight[color])
		goto out_put;

	/* this pwq is done, clear flush_color */
	pwq->flush_color = -1;

	/*
	 * If this was the last pwq, wake up the first flusher.  It
	 * will handle the rest.
	 */
	if (atomic_dec_and_test(&pwq->wq->nr_pwqs_to_flush))
		complete(&pwq->wq->first_flusher->done);
out_put:
	put_pwq(pwq);
}