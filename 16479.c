static int get_task_root(struct task_struct *task, struct path *root)
{
	int result = -ENOENT;

	task_lock(task);
	if (task->fs) {
		get_fs_root(task->fs, root);
		result = 0;
	}
	task_unlock(task);
	return result;
}