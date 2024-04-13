void kthread_destroy_worker(struct kthread_worker *worker)
{
	struct task_struct *task;

	task = worker->task;
	if (WARN_ON(!task))
		return;

	kthread_flush_worker(worker);
	kthread_stop(task);
	WARN_ON(!list_empty(&worker->work_list));
	kfree(worker);
}