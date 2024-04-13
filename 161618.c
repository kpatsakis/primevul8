static bool flush_workqueue_prep_pwqs(struct workqueue_struct *wq,
				      int flush_color, int work_color)
{
	bool wait = false;
	struct pool_workqueue *pwq;

	if (flush_color >= 0) {
		WARN_ON_ONCE(atomic_read(&wq->nr_pwqs_to_flush));
		atomic_set(&wq->nr_pwqs_to_flush, 1);
	}

	for_each_pwq(pwq, wq) {
		struct worker_pool *pool = pwq->pool;

		spin_lock_irq(&pool->lock);

		if (flush_color >= 0) {
			WARN_ON_ONCE(pwq->flush_color != -1);

			if (pwq->nr_in_flight[flush_color]) {
				pwq->flush_color = flush_color;
				atomic_inc(&wq->nr_pwqs_to_flush);
				wait = true;
			}
		}

		if (work_color >= 0) {
			WARN_ON_ONCE(work_color != work_next_color(pwq->work_color));
			pwq->work_color = work_color;
		}

		spin_unlock_irq(&pool->lock);
	}

	if (flush_color >= 0 && atomic_dec_and_test(&wq->nr_pwqs_to_flush))
		complete(&wq->first_flusher->done);

	return wait;
}