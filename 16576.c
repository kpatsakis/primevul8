static struct dentry *proc_lookupfdinfo(struct inode *dir,
					struct dentry *dentry,
					struct nameidata *nd)
{
	return proc_lookupfd_common(dir, dentry, proc_fdinfo_instantiate);
}