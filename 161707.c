__kthread_create_worker(int cpu, unsigned int flags,
			const char namefmt[], va_list args)
{
	struct kthread_worker *worker;
	struct task_struct *task;
	int node = -1;

	worker = kzalloc(sizeof(*worker), GFP_KERNEL);
	if (!worker)
		return ERR_PTR(-ENOMEM);

	kthread_init_worker(worker);

	if (cpu >= 0)
		node = cpu_to_node(cpu);

	task = __kthread_create_on_node(kthread_worker_fn, worker,
						node, namefmt, args);
	if (IS_ERR(task))
		goto fail_task;

	if (cpu >= 0)
		kthread_bind(task, cpu);

	worker->flags = flags;
	worker->task = task;
	wake_up_process(task);
	return worker;

fail_task:
	kfree(worker);
	return ERR_CAST(task);
}