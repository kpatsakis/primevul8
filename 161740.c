signed long __sched schedule_timeout_idle(signed long timeout)
{
	__set_current_state(TASK_IDLE);
	return schedule_timeout(timeout);
}