void print_worker_info(const char *log_lvl, struct task_struct *task)
{
	work_func_t *fn = NULL;
	char name[WQ_NAME_LEN] = { };
	char desc[WORKER_DESC_LEN] = { };
	struct pool_workqueue *pwq = NULL;
	struct workqueue_struct *wq = NULL;
	bool desc_valid = false;
	struct worker *worker;

	if (!(task->flags & PF_WQ_WORKER))
		return;

	/*
	 * This function is called without any synchronization and @task
	 * could be in any state.  Be careful with dereferences.
	 */
	worker = kthread_probe_data(task);

	/*
	 * Carefully copy the associated workqueue's workfn and name.  Keep
	 * the original last '\0' in case the original contains garbage.
	 */
	probe_kernel_read(&fn, &worker->current_func, sizeof(fn));
	probe_kernel_read(&pwq, &worker->current_pwq, sizeof(pwq));
	probe_kernel_read(&wq, &pwq->wq, sizeof(wq));
	probe_kernel_read(name, wq->name, sizeof(name) - 1);

	/* copy worker description */
	probe_kernel_read(&desc_valid, &worker->desc_valid, sizeof(desc_valid));
	if (desc_valid)
		probe_kernel_read(desc, worker->desc, sizeof(desc) - 1);

	if (fn || name[0] || desc[0]) {
		printk("%sWorkqueue: %s %pf", log_lvl, name, fn);
		if (desc[0])
			pr_cont(" (%s)", desc);
		pr_cont("\n");
	}
}