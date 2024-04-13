static ssize_t oom_adjust_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file->f_path.dentry->d_inode);
	char buffer[PROC_NUMBUF];
	size_t len;
	int oom_adjust = OOM_DISABLE;
	unsigned long flags;

	if (!task)
		return -ESRCH;

	if (lock_task_sighand(task, &flags)) {
		oom_adjust = task->signal->oom_adj;
		unlock_task_sighand(task, &flags);
	}

	put_task_struct(task);

	len = snprintf(buffer, sizeof(buffer), "%i\n", oom_adjust);

	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}