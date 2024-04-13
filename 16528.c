proc_map_files_instantiate(struct inode *dir, struct dentry *dentry,
			   struct task_struct *task, const void *ptr)
{
	const struct file *file = ptr;
	struct proc_inode *ei;
	struct inode *inode;

	if (!file)
		return ERR_PTR(-ENOENT);

	inode = proc_pid_make_inode(dir->i_sb, task);
	if (!inode)
		return ERR_PTR(-ENOENT);

	ei = PROC_I(inode);
	ei->op.proc_get_link = proc_map_files_get_link;

	inode->i_op = &proc_pid_link_inode_operations;
	inode->i_size = 64;
	inode->i_mode = S_IFLNK;

	if (file->f_mode & FMODE_READ)
		inode->i_mode |= S_IRUSR;
	if (file->f_mode & FMODE_WRITE)
		inode->i_mode |= S_IWUSR;

	d_set_d_op(dentry, &tid_map_files_dentry_operations);
	d_add(dentry, inode);

	return NULL;
}