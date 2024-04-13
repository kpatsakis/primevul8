void kthread_bind_mask(struct task_struct *p, const struct cpumask *mask)
{
	__kthread_bind_mask(p, mask, TASK_UNINTERRUPTIBLE);
}