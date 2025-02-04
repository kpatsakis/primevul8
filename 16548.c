static int proc_pid_schedstat(struct task_struct *task, char *buffer)
{
	return sprintf(buffer, "%llu %llu %lu\n",
			(unsigned long long)task->se.sum_exec_runtime,
			(unsigned long long)task->sched_info.run_delay,
			task->sched_info.pcount);
}