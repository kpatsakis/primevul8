int kthread_worker_fn(void *worker_ptr)
{
	struct kthread_worker *worker = worker_ptr;
	struct kthread_work *work;

	/*
	 * FIXME: Update the check and remove the assignment when all kthread
	 * worker users are created using kthread_create_worker*() functions.
	 */
	WARN_ON(worker->task && worker->task != current);
	worker->task = current;

	if (worker->flags & KTW_FREEZABLE)
		set_freezable();

repeat:
	set_current_state(TASK_INTERRUPTIBLE);	/* mb paired w/ kthread_stop */

	if (kthread_should_stop()) {
		__set_current_state(TASK_RUNNING);
		spin_lock_irq(&worker->lock);
		worker->task = NULL;
		spin_unlock_irq(&worker->lock);
		return 0;
	}

	work = NULL;
	spin_lock_irq(&worker->lock);
	if (!list_empty(&worker->work_list)) {
		work = list_first_entry(&worker->work_list,
					struct kthread_work, node);
		list_del_init(&work->node);
	}
	worker->current_work = work;
	spin_unlock_irq(&worker->lock);

	if (work) {
		__set_current_state(TASK_RUNNING);
		work->func(work);
	} else if (!freezing(current))
		schedule();

	try_to_freeze();
	goto repeat;
}