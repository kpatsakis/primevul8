static void __kthread_bind(struct task_struct *p, unsigned int cpu, long state)
{
	__kthread_bind_mask(p, cpumask_of(cpu), state);
}