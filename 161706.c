void wq_worker_waking_up(struct task_struct *task, int cpu)
{
	struct worker *worker = kthread_data(task);

	if (!(worker->flags & WORKER_NOT_RUNNING)) {
		WARN_ON_ONCE(worker->pool->cpu != cpu);
		atomic_inc(&worker->pool->nr_running);
	}
}