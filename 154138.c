void set_curr_task(int cpu, struct task_struct *p)
{
	cpu_curr(cpu) = p;
}