inline int task_curr(const struct task_struct *p)
{
	return cpu_curr(task_cpu(p)) == p;
}