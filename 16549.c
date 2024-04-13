static int proc_single_show(struct seq_file *m, void *v)
{
	struct inode *inode = m->private;
	struct pid_namespace *ns;
	struct pid *pid;
	struct task_struct *task;
	int ret;

	ns = inode->i_sb->s_fs_info;
	pid = proc_pid(inode);
	task = get_pid_task(pid, PIDTYPE_PID);
	if (!task)
		return -ESRCH;

	ret = PROC_I(inode)->op.proc_show(m, ns, pid, task);

	put_task_struct(task);
	return ret;
}