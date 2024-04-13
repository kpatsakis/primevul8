void kthread_delayed_work_timer_fn(unsigned long __data)
{
	struct kthread_delayed_work *dwork =
		(struct kthread_delayed_work *)__data;
	struct kthread_work *work = &dwork->work;
	struct kthread_worker *worker = work->worker;

	/*
	 * This might happen when a pending work is reinitialized.
	 * It means that it is used a wrong way.
	 */
	if (WARN_ON_ONCE(!worker))
		return;

	spin_lock(&worker->lock);
	/* Work must not be used with >1 worker, see kthread_queue_work(). */
	WARN_ON_ONCE(work->worker != worker);

	/* Move the work from worker->delayed_work_list. */
	WARN_ON_ONCE(list_empty(&work->node));
	list_del_init(&work->node);
	kthread_insert_work(worker, work, &worker->work_list);

	spin_unlock(&worker->lock);
}