static int proc_tgid_io_accounting(struct task_struct *task, char *buffer)
{
	return do_io_accounting(task, buffer, 1);
}