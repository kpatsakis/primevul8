void *kthread_data(struct task_struct *task)
{
	return to_kthread(task)->data;
}