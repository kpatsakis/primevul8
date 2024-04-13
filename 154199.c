int task_prio(const struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}