static struct dentry *proc_lookupfd(struct inode *dir, struct dentry *dentry,
				    struct nameidata *nd)
{
	return proc_lookupfd_common(dir, dentry, proc_fd_instantiate);
}