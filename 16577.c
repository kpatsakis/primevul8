static int proc_cwd_link(struct dentry *dentry, struct path *path)
{
	struct task_struct *task = get_proc_task(dentry->d_inode);
	int result = -ENOENT;

	if (task) {
		task_lock(task);
		if (task->fs) {
			get_fs_pwd(task->fs, path);
			result = 0;
		}
		task_unlock(task);
		put_task_struct(task);
	}
	return result;
}