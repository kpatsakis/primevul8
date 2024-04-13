struct task_struct *idle_task(int cpu)
{
	return cpu_rq(cpu)->idle;
}