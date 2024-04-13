void kthread_bind(struct task_struct *p, unsigned int cpu)
{
	__kthread_bind(p, cpu, TASK_UNINTERRUPTIBLE);
}