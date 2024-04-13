void flush_workqueue(struct workqueue_struct *wq)
{
	struct wq_flusher this_flusher = {
		.list = LIST_HEAD_INIT(this_flusher.list),
		.flush_color = -1,
		.done = COMPLETION_INITIALIZER_ONSTACK(this_flusher.done),
	};
	int next_color;

	if (WARN_ON(!wq_online))
		return;

	lock_map_acquire(&wq->lockdep_map);
	lock_map_release(&wq->lockdep_map);

	mutex_lock(&wq->mutex);

	/*
	 * Start-to-wait phase
	 */
	next_color = work_next_color(wq->work_color);

	if (next_color != wq->flush_color) {
		/*
		 * Color space is not full.  The current work_color
		 * becomes our flush_color and work_color is advanced
		 * by one.
		 */
		WARN_ON_ONCE(!list_empty(&wq->flusher_overflow));
		this_flusher.flush_color = wq->work_color;
		wq->work_color = next_color;

		if (!wq->first_flusher) {
			/* no flush in progress, become the first flusher */
			WARN_ON_ONCE(wq->flush_color != this_flusher.flush_color);

			wq->first_flusher = &this_flusher;

			if (!flush_workqueue_prep_pwqs(wq, wq->flush_color,
						       wq->work_color)) {
				/* nothing to flush, done */
				wq->flush_color = next_color;
				wq->first_flusher = NULL;
				goto out_unlock;
			}
		} else {
			/* wait in queue */
			WARN_ON_ONCE(wq->flush_color == this_flusher.flush_color);
			list_add_tail(&this_flusher.list, &wq->flusher_queue);
			flush_workqueue_prep_pwqs(wq, -1, wq->work_color);
		}
	} else {
		/*
		 * Oops, color space is full, wait on overflow queue.
		 * The next flush completion will assign us
		 * flush_color and transfer to flusher_queue.
		 */
		list_add_tail(&this_flusher.list, &wq->flusher_overflow);
	}

	check_flush_dependency(wq, NULL);

	mutex_unlock(&wq->mutex);

	wait_for_completion(&this_flusher.done);

	/*
	 * Wake-up-and-cascade phase
	 *
	 * First flushers are responsible for cascading flushes and
	 * handling overflow.  Non-first flushers can simply return.
	 */
	if (wq->first_flusher != &this_flusher)
		return;

	mutex_lock(&wq->mutex);

	/* we might have raced, check again with mutex held */
	if (wq->first_flusher != &this_flusher)
		goto out_unlock;

	wq->first_flusher = NULL;

	WARN_ON_ONCE(!list_empty(&this_flusher.list));
	WARN_ON_ONCE(wq->flush_color != this_flusher.flush_color);

	while (true) {
		struct wq_flusher *next, *tmp;

		/* complete all the flushers sharing the current flush color */
		list_for_each_entry_safe(next, tmp, &wq->flusher_queue, list) {
			if (next->flush_color != wq->flush_color)
				break;
			list_del_init(&next->list);
			complete(&next->done);
		}

		WARN_ON_ONCE(!list_empty(&wq->flusher_overflow) &&
			     wq->flush_color != work_next_color(wq->work_color));

		/* this flush_color is finished, advance by one */
		wq->flush_color = work_next_color(wq->flush_color);

		/* one color has been freed, handle overflow queue */
		if (!list_empty(&wq->flusher_overflow)) {
			/*
			 * Assign the same color to all overflowed
			 * flushers, advance work_color and append to
			 * flusher_queue.  This is the start-to-wait
			 * phase for these overflowed flushers.
			 */
			list_for_each_entry(tmp, &wq->flusher_overflow, list)
				tmp->flush_color = wq->work_color;

			wq->work_color = work_next_color(wq->work_color);

			list_splice_tail_init(&wq->flusher_overflow,
					      &wq->flusher_queue);
			flush_workqueue_prep_pwqs(wq, -1, wq->work_color);
		}

		if (list_empty(&wq->flusher_queue)) {
			WARN_ON_ONCE(wq->flush_color != wq->work_color);
			break;
		}

		/*
		 * Need to flush more colors.  Make the next flusher
		 * the new first flusher and arm pwqs.
		 */
		WARN_ON_ONCE(wq->flush_color == wq->work_color);
		WARN_ON_ONCE(wq->flush_color != next->flush_color);

		list_del_init(&next->list);
		wq->first_flusher = next;

		if (flush_workqueue_prep_pwqs(wq, wq->flush_color, -1))
			break;

		/*
		 * Meh... this color is already done, clear first
		 * flusher and repeat cascading.
		 */
		wq->first_flusher = NULL;
	}

out_unlock:
	mutex_unlock(&wq->mutex);
}