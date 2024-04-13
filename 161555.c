static bool __kthread_cancel_work_sync(struct kthread_work *work, bool is_dwork)
{
	struct kthread_worker *worker = work->worker;
	unsigned long flags;
	int ret = false;

	if (!worker)
		goto out;

	spin_lock_irqsave(&worker->lock, flags);
	/* Work must not be used with >1 worker, see kthread_queue_work(). */
	WARN_ON_ONCE(work->worker != worker);

	ret = __kthread_cancel_work(work, is_dwork, &flags);

	if (worker->current_work != work)
		goto out_fast;

	/*
	 * The work is in progress and we need to wait with the lock released.
	 * In the meantime, block any queuing by setting the canceling counter.
	 */
	work->canceling++;
	spin_unlock_irqrestore(&worker->lock, flags);
	kthread_flush_work(work);
	spin_lock_irqsave(&worker->lock, flags);
	work->canceling--;

out_fast:
	spin_unlock_irqrestore(&worker->lock, flags);
out:
	return ret;
}