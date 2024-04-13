signed long __sched schedule_timeout_killable(signed long timeout)
{
	__set_current_state(TASK_KILLABLE);
	return schedule_timeout(timeout);
}