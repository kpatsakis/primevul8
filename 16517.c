static int proc_pid_syscall(struct task_struct *task, char *buffer)
{
	long nr;
	unsigned long args[6], sp, pc;
	int res = lock_trace(task);
	if (res)
		return res;

	if (task_current_syscall(task, &nr, args, 6, &sp, &pc))
		res = sprintf(buffer, "running\n");
	else if (nr < 0)
		res = sprintf(buffer, "%ld 0x%lx 0x%lx\n", nr, sp, pc);
	else
		res = sprintf(buffer,
		       "%ld 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",
		       nr,
		       args[0], args[1], args[2], args[3], args[4], args[5],
		       sp, pc);
	unlock_trace(task);
	return res;
}