void __sched yield(void)
{
	set_current_state(TASK_RUNNING);
	sys_sched_yield();
}