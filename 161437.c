bool kthread_mod_delayed_work(struct kthread_worker *worker,
			      struct kthread_delayed_work *dwork,
			      unsigned long delay)
{
	struct kthread_work *work = &dwork->work;
	unsigned long flags;
	int ret = false;

	spin_lock_irqsave(&worker->lock, flags);

	/* Do not bother with canceling when never queued. */
	if (!work->worker)
		goto fast_queue;

	/* Work must not be used with >1 worker, see kthread_queue_work() */
	WARN_ON_ONCE(work->worker != worker);

	/* Do not fight with another command that is canceling this work. */
	if (work->canceling)
		goto out;

	ret = __kthread_cancel_work(work, true, &flags);
fast_queue:
	__kthread_queue_delayed_work(worker, dwork, delay);
out:
	spin_unlock_irqrestore(&worker->lock, flags);
	return ret;
}