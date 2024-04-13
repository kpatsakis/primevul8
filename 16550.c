static int proc_task_readdir(struct file * filp, void * dirent, filldir_t filldir)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	struct task_struct *leader = NULL;
	struct task_struct *task;
	int retval = -ENOENT;
	ino_t ino;
	int tid;
	struct pid_namespace *ns;

	task = get_proc_task(inode);
	if (!task)
		goto out_no_task;
	rcu_read_lock();
	if (pid_alive(task)) {
		leader = task->group_leader;
		get_task_struct(leader);
	}
	rcu_read_unlock();
	put_task_struct(task);
	if (!leader)
		goto out_no_task;
	retval = 0;

	switch ((unsigned long)filp->f_pos) {
	case 0:
		ino = inode->i_ino;
		if (filldir(dirent, ".", 1, filp->f_pos, ino, DT_DIR) < 0)
			goto out;
		filp->f_pos++;
		/* fall through */
	case 1:
		ino = parent_ino(dentry);
		if (filldir(dirent, "..", 2, filp->f_pos, ino, DT_DIR) < 0)
			goto out;
		filp->f_pos++;
		/* fall through */
	}

	/* f_version caches the tgid value that the last readdir call couldn't
	 * return. lseek aka telldir automagically resets f_version to 0.
	 */
	ns = filp->f_dentry->d_sb->s_fs_info;
	tid = (int)filp->f_version;
	filp->f_version = 0;
	for (task = first_tid(leader, tid, filp->f_pos - 2, ns);
	     task;
	     task = next_tid(task), filp->f_pos++) {
		tid = task_pid_nr_ns(task, ns);
		if (proc_task_fill_cache(filp, dirent, filldir, task, tid) < 0) {
			/* returning this tgid failed, save it as the first
			 * pid for the next readir call */
			filp->f_version = (u64)tid;
			put_task_struct(task);
			break;
		}
	}
out:
	put_task_struct(leader);
out_no_task:
	return retval;
}