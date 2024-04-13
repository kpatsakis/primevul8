static bool __kthread_cancel_work(struct kthread_work *work, bool is_dwork,
				  unsigned long *flags)
{
	/* Try to cancel the timer if exists. */
	if (is_dwork) {
		struct kthread_delayed_work *dwork =
			container_of(work, struct kthread_delayed_work, work);
		struct kthread_worker *worker = work->worker;

		/*
		 * del_timer_sync() must be called to make sure that the timer
		 * callback is not running. The lock must be temporary released
		 * to avoid a deadlock with the callback. In the meantime,
		 * any queuing is blocked by setting the canceling counter.
		 */
		work->canceling++;
		spin_unlock_irqrestore(&worker->lock, *flags);
		del_timer_sync(&dwork->timer);
		spin_lock_irqsave(&worker->lock, *flags);
		work->canceling--;
	}

	/*
	 * Try to remove the work from a worker list. It might either
	 * be from worker->work_list or from worker->delayed_work_list.
	 */
	if (!list_empty(&work->node)) {
		list_del_init(&work->node);
		return true;
	}

	return false;
}