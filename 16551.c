static int proc_fd_link(struct dentry *dentry, struct path *path)
{
	return proc_fd_info(dentry->d_inode, path, NULL);
}