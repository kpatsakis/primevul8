struct task_struct *curr_task(int cpu)
{
	return cpu_curr(cpu);
}