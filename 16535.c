sched_autogroup_write(struct file *file, const char __user *buf,
	    size_t count, loff_t *offset)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	struct task_struct *p;
	char buffer[PROC_NUMBUF];
	int nice;
	int err;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	err = kstrtoint(strstrip(buffer), 0, &nice);
	if (err < 0)
		return err;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	err = nice;
	err = proc_sched_autogroup_set_nice(p, &err);
	if (err)
		count = err;

	put_task_struct(p);

	return count;
}