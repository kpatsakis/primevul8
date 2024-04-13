void kthread_flush_work(struct kthread_work *work)
{
	struct kthread_flush_work fwork = {
		KTHREAD_WORK_INIT(fwork.work, kthread_flush_work_fn),
		COMPLETION_INITIALIZER_ONSTACK(fwork.done),
	};
	struct kthread_worker *worker;
	bool noop = false;

	worker = work->worker;
	if (!worker)
		return;

	spin_lock_irq(&worker->lock);
	/* Work must not be used with >1 worker, see kthread_queue_work(). */
	WARN_ON_ONCE(work->worker != worker);

	if (!list_empty(&work->node))
		kthread_insert_work(worker, &fwork.work, work->node.next);
	else if (worker->current_work == work)
		kthread_insert_work(worker, &fwork.work,
				    worker->work_list.next);
	else
		noop = true;

	spin_unlock_irq(&worker->lock);

	if (!noop)
		wait_for_completion(&fwork.done);
}