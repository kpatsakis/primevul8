static ssize_t comm_write(struct file *file, const char __user *buf,
				size_t count, loff_t *offset)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	struct task_struct *p;
	char buffer[TASK_COMM_LEN];

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	if (same_thread_group(current, p))
		set_task_comm(p, buffer);
	else
		count = -EINVAL;

	put_task_struct(p);

	return count;
}