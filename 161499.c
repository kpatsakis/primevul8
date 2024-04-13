void *kthread_probe_data(struct task_struct *task)
{
	struct kthread *kthread = to_kthread(task);
	void *data = NULL;

	probe_kernel_read(&data, &kthread->data, sizeof(data));
	return data;
}